// A pair of pipes (top and bottom) with a gap in the middle.
#pragma once
#include "raylib.h"

class Pipe {
public:
    float x;               // left edge
    float gapY;            // centre of the gap
    bool passed = false;   // already scored?

    explicit Pipe(float x);
    Rectangle top() const;
    Rectangle bottom() const;
    bool hits(Rectangle box) const;
    void draw() const;
};
