#include "Bird.h"

#include <algorithm>
#include <cmath>

#include "Config.h"
#include "rlgl.h"

using namespace cfg;

Bird::Bird() : y(H / 2.0f - 60) {}

void Bird::flap() {
    vy = FLAP;
    flapTimer = 10;
}

void Bird::fall(float maxFall) {
    vy = std::min(vy + GRAVITY, maxFall);
    y = std::min(y + vy, FLOOR - R);
    flapTimer = std::max(0, flapTimer - 1);
}

Rectangle Bird::hitbox() const {
    return {x - R + 3, y - R + 3, 2 * R - 6, 2 * R - 6};
}

void Bird::draw(int direction, int tick) const {
    // Tilt up when rising, down when falling, and face the way we fly.
    float tilt = std::clamp(-vy * 4, -70.0f, 25.0f);

    rlPushMatrix();
    rlTranslatef(x, y, 0);
    rlRotatef(-tilt * direction, 0, 0, 1);
    rlScalef(direction, 1, 1);   // mirror the bird when flying left

    // Everything below is drawn around (0, 0), the centre of the bird.
    DrawCircle(0, 0, R, BIRD);
    DrawRing({0, 0}, R - 2, R, 0, 360, 36, BLACK);

    float wingY = flapTimer ? -3 : 3 + int(3 * std::sin(tick * 0.3f));
    DrawEllipse(-8, wingY, 8, 5, WHITE);
    DrawEllipseLines(-8, wingY, 8, 5, BLACK);

    DrawCircle(7, -6, 7, WHITE);
    DrawRing({7, -6}, 5, 7, 0, 360, 24, BLACK);
    DrawCircle(9, -6, 3, BLACK);

    Vector2 a = {10, 1}, b = {24, 5}, c = {10, 10};   // beak
    DrawTriangle(a, c, b, BEAK);
    DrawTriangleLines(a, c, b, BLACK);

    rlPopMatrix();
}
