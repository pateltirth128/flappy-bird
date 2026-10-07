// The player: a yellow bird that falls with gravity and flaps upward.
#pragma once
#include "raylib.h"

class Bird {
public:
    float x = 40.0f;   // starts at the left edge
    float y = 0.0f;
    float vy = 0.0f;   // vertical speed (negative = going up)

    Bird();
    void flap();
    void fall(float maxFall);                 // apply gravity for one frame
    Rectangle hitbox() const;                 // a bit smaller than the drawing, to be fair
    void draw(int direction, int tick) const;  // direction: 1 = right, -1 = left

private:
    int flapTimer = 0;   // > 0 for a few frames after a flap (wing goes up)
};
