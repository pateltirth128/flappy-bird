#include "Pipe.h"

#include "Config.h"

using namespace cfg;

Pipe::Pipe(float x) : x(x) {
    gapY = GetRandomValue(80 + GAP / 2, FLOOR - 80 - GAP / 2);
}

Rectangle Pipe::top() const {
    return {x, 0, PIPE_W, gapY - GAP / 2};
}

Rectangle Pipe::bottom() const {
    float y = gapY + GAP / 2;
    return {x, y, PIPE_W, FLOOR - y};
}

bool Pipe::hits(Rectangle box) const {
    return CheckCollisionRecs(box, top()) || CheckCollisionRecs(box, bottom());
}

// One green block with a light stripe and a dark outline.
static void drawBlock(Rectangle r) {
    DrawRectangleRec(r, PIPE);
    DrawRectangleRec({r.x + 8, r.y, 10, r.height}, PIPE_LIGHT);
    DrawRectangleLinesEx(r, 3, PIPE_DARK);
}

void Pipe::draw() const {
    Rectangle t = top(), b = bottom();
    drawBlock(t);
    drawBlock(b);
    drawBlock({x - 6, t.y + t.height - 30, PIPE_W + 12, 30});   // cap at the bottom of the top pipe
    drawBlock({x - 6, b.y, PIPE_W + 12, 30});                   // cap at the top of the bottom pipe
}
