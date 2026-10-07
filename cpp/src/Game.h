// The whole game: owns the bird and the pipes, and runs the main loop.
#pragma once
#include <vector>

#include "Bird.h"
#include "Pipe.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    enum class State { Ready, Playing, Dead };

    Bird bird;
    std::vector<Pipe> pipes;
    State state = State::Ready;
    int direction = 1;        // 1 = flying right, -1 = flying left
    int score = 0;
    int best = 0;
    int tick = 0;             // frame counter, used for animations
    int turnMessage = 0;      // frames left to show "TURN AROUND!"
    bool turnPending = false;
    bool paused = false;
    float scroll = 0.0f;      // how far the world has moved (for clouds and ground)

    void reset();
    void handleInput();
    void update();
    void movePipes();
    void checkPipes();
    void die();

    bool isBehind(const Pipe& p) const;
    bool isBetweenPipes() const;
    void turnAround();

    void draw() const;
    void drawBackground() const;
    void drawText() const;
};
