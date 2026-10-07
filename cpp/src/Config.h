// All the game settings in one place. Change a number here to tweak the game.
#pragma once
#include "raylib.h"

namespace cfg {

// Window
constexpr int W = 600;
constexpr int H = 800;
constexpr float FLOOR = H - 100;        // y of the ground

// Bird physics (per frame, at 60 updates a second)
constexpr float GRAVITY = 0.45f;
constexpr float FLAP = -8.5f;            // upward kick when you flap
constexpr float MAX_FALL = 10.0f;
constexpr float R = 17.0f;               // bird radius

// World
constexpr float SPEED = 3.2f;            // how fast the pipes move
constexpr int TURN_EVERY = 5;            // turn around every N points
constexpr float PIPE_W = 80.0f;
constexpr float GAP = 190.0f;            // size of the hole in a pipe
constexpr float SPACING = 280.0f;        // distance between pipes

// Colours
constexpr Color SKY_TOP = {78, 192, 202, 255};
constexpr Color SKY_BOTTOM = {180, 235, 240, 255};
constexpr Color PIPE = {94, 190, 60, 255};
constexpr Color PIPE_DARK = {58, 130, 36, 255};
constexpr Color PIPE_LIGHT = {150, 225, 110, 255};
constexpr Color SAND = {222, 216, 149, 255};
constexpr Color GRASS = {116, 191, 46, 255};
constexpr Color STRIPE = {200, 190, 120, 255};
constexpr Color BIRD = {250, 200, 40, 255};
constexpr Color BEAK = {240, 120, 30, 255};
constexpr Color CLOUD = {240, 250, 252, 255};

constexpr const char* AUTHOR = "Tirth Patel";
constexpr const char* HIGHSCORE_FILE = "highscore.txt";

}  // namespace cfg
