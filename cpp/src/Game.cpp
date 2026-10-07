#include "Game.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>

#include "Config.h"
#include "rlgl.h"

using namespace cfg;

// ---------- setup ----------

Game::Game() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);   // smooth, no tearing
    InitWindow(W, H, TextFormat("Flappy Bird | by %s", AUTHOR));
    rlDisableBackfaceCulling();   // lets us mirror the bird when it flies left
    SetRandomSeed((unsigned)time(nullptr));

    std::ifstream file(HIGHSCORE_FILE);
    file >> best;
    reset();
}

Game::~Game() {
    CloseWindow();
}

void Game::reset() {
    bird = Bird();
    pipes.clear();
    for (int i = 0; i < 3; i++) pipes.emplace_back(450 + i * SPACING);
    state = State::Ready;
    direction = 1;
    score = tick = turnMessage = 0;
    scroll = 0;
    turnPending = paused = false;
}

// ---------- main loop ----------

void Game::run() {
    // The game logic runs exactly 60 times a second, no matter how fast
    // the screen refreshes, so it plays the same speed on every PC.
    const float STEP = 1.0f / 60.0f;
    float lag = 0;

    while (!WindowShouldClose()) {   // Esc or the X button closes it
        handleInput();
        lag += std::min(GetFrameTime(), 0.25f);
        while (lag >= STEP) {
            update();
            lag -= STEP;
        }
        draw();
    }
}

void Game::handleInput() {
    if (IsKeyPressed(KEY_P) && state == State::Playing) paused = !paused;

    bool flapPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) ||
                       IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (!flapPressed) return;

    if (state == State::Dead) {
        if (bird.y >= FLOOR - R) reset();   // wait until the bird hits the ground
    } else if (!paused) {
        state = State::Playing;
        bird.flap();
    }
}

void Game::update() {
    tick++;
    if (paused) return;
    turnMessage = std::max(0, turnMessage - 1);

    if (state == State::Ready) {   // bob up and down on the start screen
        bird.y = H / 2.0f - 60 + 8 * std::sin(tick * 0.08f);
        return;
    }
    if (state == State::Dead) {   // just let the bird drop
        bird.fall(MAX_FALL + 4);
        return;
    }

    bird.fall(MAX_FALL);

    // Glide to a third of the screen from the side we came from.
    float target = W / 2.0f - direction * W / 6.0f;
    bird.x += std::clamp(target - bird.x, -SPEED, SPEED);
    scroll += direction * SPEED;

    movePipes();
    checkPipes();
    if (state != State::Playing) return;

    if (turnPending && isBetweenPipes()) turnAround();

    if (bird.y >= FLOOR - R) {
        die();
    } else if (bird.y < R) {   // ceiling
        bird.y = R;
        bird.vy = 0;
    }
}

// ---------- pipes ----------

void Game::movePipes() {
    for (Pipe& p : pipes) p.x -= direction * SPEED;

    // Add a new pipe ahead when the front one comes on screen.
    float frontX = pipes[0].x;
    for (const Pipe& p : pipes) {
        if (p.x * direction > frontX * direction) frontX = p.x;
    }
    if ((frontX + PIPE_W / 2 - W / 2.0f) * direction < W / 2.0f + PIPE_W) {
        pipes.emplace_back(frontX + direction * SPACING);
    }

    // Forget pipes that are far behind us.
    auto farBehind = [&](const Pipe& p) {
        return (W / 2.0f - p.x - PIPE_W / 2) * direction >= W / 2.0f + 2 * SPACING;
    };
    pipes.erase(std::remove_if(pipes.begin(), pipes.end(), farBehind), pipes.end());
}

void Game::checkPipes() {
    for (Pipe& p : pipes) {
        if (p.hits(bird.hitbox())) {
            die();
            return;
        }
        if (!p.passed && isBehind(p)) {
            p.passed = true;
            score++;
            if (score % TURN_EVERY == 0) {
                // Don't turn yet: we're still inside this pipe. Wait until we're
                // in the open space between pipes (see update()).
                turnPending = true;
                turnMessage = 90;
            }
        }
    }
}

bool Game::isBehind(const Pipe& p) const {   // has the bird fully passed this pipe?
    return (bird.x - p.x - PIPE_W / 2) * direction > PIPE_W / 2;
}

bool Game::isBetweenPipes() const {   // about halfway between two pipes?
    float needed = (SPACING - PIPE_W) / 2 - R - 4;
    for (const Pipe& p : pipes) {
        float gap = std::abs(p.x + PIPE_W / 2 - bird.x) - PIPE_W / 2 - R;
        if (gap < needed) return false;
    }
    return true;
}

void Game::turnAround() {
    direction = -direction;
    turnPending = false;
    turnMessage = 90;
    for (Pipe& p : pipes) p.passed = isBehind(p);   // pipes behind us now count again
}

void Game::die() {
    state = State::Dead;
    if (score > best) {
        best = score;
        std::ofstream(HIGHSCORE_FILE) << best;
    }
}

// ---------- drawing ----------

void Game::draw() const {
    BeginDrawing();
    drawBackground();
    for (const Pipe& p : pipes) p.draw();
    bird.draw(direction, tick);
    drawText();
    EndDrawing();
}

void Game::drawBackground() const {
    DrawRectangleGradientV(0, 0, W, FLOOR, SKY_TOP, SKY_BOTTOM);

    // Clouds move slower than the pipes, which gives a sense of depth.
    float offset = scroll * 0.3f;
    int first = (int)std::floor(offset / 260);
    for (int i = first - 1; i <= first + 3; i++) {
        unsigned h = (unsigned)i * 2654435761u;   // same "random" cloud for the same i
        float x = i * 260 - offset + h % 81;
        float y = 60 + (h >> 8) % 221;
        DrawCircle(x, y, 28, CLOUD);
        DrawCircle(x + 30, y - 12, 34, CLOUD);
        DrawCircle(x + 62, y, 26, CLOUD);
        DrawCircle(x + 30, y + 8, 26, CLOUD);
    }

    // Ground with moving stripes.
    DrawRectangle(0, FLOOR, W, 100, SAND);
    DrawRectangle(0, FLOOR, W, 16, GRASS);
    int shift = ((int)scroll % 40 + 40) % 40;
    for (int x = -40 - shift; x < W + 40; x += 40) {
        Vector2 a = {(float)x, FLOOR + 16}, b = {x + 20.0f, FLOOR + 16};
        Vector2 c = {x + 10.0f, FLOOR + 30}, d = {x - 10.0f, FLOOR + 30};
        DrawTriangle(a, d, c, STRIPE);
        DrawTriangle(a, c, b, STRIPE);
    }
}

// White text with a black shadow, centred on the screen.
static void centredText(const char* text, int size, float y) {
    int x = W / 2 - MeasureText(text, size) / 2;
    int top = (int)y - size / 2;
    DrawText(text, x + 3, top + 3, size, BLACK);
    DrawText(text, x, top, size, WHITE);
}

void Game::drawText() const {
    if (state == State::Ready) {
        centredText("FLAPPY BIRD", 64, 180);
        centredText("Fly from left to right!", 30, 250);
        centredText(TextFormat("Made by %s", AUTHOR), 22, 290);
        centredText("SPACE / Click to flap", 22, 520);
        centredText(TextFormat("Turns around every %d points", TURN_EVERY), 22, 555);
        centredText(TextFormat("Best: %d", best), 22, 590);
    } else {
        centredText(TextFormat("%d", score), 64, 70);
    }

    if (turnMessage) centredText("TURN AROUND!", 30, 140);

    if (state == State::Dead) {
        DrawRectangle(W / 2 - 190, 220, 380, 220, {0, 0, 0, 140});
        centredText("GAME OVER", 64, 270);
        centredText(TextFormat("Score: %d   Best: %d", score, best), 30, 340);
        if (bird.y >= FLOOR - R) centredText("SPACE / Click to restart", 22, 395);
        centredText(TextFormat("Made by %s", AUTHOR), 22, H - 40);
    }

    if (paused) centredText("PAUSED", 64, H / 2.0f);
}
