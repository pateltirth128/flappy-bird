# 🐦 Flappy Bird – Left to Right (and back!)

![Python](https://img.shields.io/badge/Python-3.8%2B-blue?logo=python&logoColor=white)
![pygame](https://img.shields.io/badge/pygame-2.x-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

A twist on the classic Flappy Bird, built with **Python** and **pygame**.
The bird starts on the **left** and flies **right**, and every **5 points** it **turns around** and flies back the other way through the pipes.

No image files needed: everything (bird, pipes, clouds, ground) is drawn with code.

---

## 🎮 Gameplay

1. The bird waits on the left side of the screen. Press **Space** to start.
2. Flap through the gaps between the pipes. Each pipe you pass is **+1 point**.
3. At **5, 10, 15, ...** points, the bird **turns around** and you fly back through the pipes you just passed.
4. Hit a pipe or the ground and it's game over. Beat your **high score**!

## ✨ Features

- Bird starts on the left and flies left to right
- Changes direction every 5 points, with a **TURN AROUND!** alert
- Bird flips to face the way it is flying and tilts when rising or falling
- Endless, randomly generated pipes
- Live score and a saved high score (`highscore.txt`)
- Parallax clouds and a scrolling ground
- Pause, restart, and a start screen with credits

## 🚀 Getting started

**Requirements:** Python 3.8+ and pygame

```bash
git clone https://github.com/pateltirth128/flappy-bird.git
cd flappy-bird
pip install -r requirements.txt
python flappy_bird.py
```

> **Windows tip:** if `python` opens the Microsoft Store or says *"Python was not found"*, search Windows for **Manage app execution aliases**, turn off **python.exe** and **python3.exe**, then restart your terminal.

## ⌨️ Controls

| Key | Action |
|-----|--------|
| `SPACE` / `↑` / Left click | Flap, start, and restart |
| `P` | Pause / resume |
| `ESC` | Quit |

## 🧠 How it works

The game runs a loop **60 times per second**:

```
events()  →  update()  →  draw()
 input       physics       graphics
```

- **`events()`** reads the keyboard and mouse.
- **`update()`** applies gravity, moves the pipes, checks collisions, and counts points.
- **`draw()`** draws the sky, clouds, pipes, ground, bird, and text.

**Changing direction:** a single variable `d` stores the direction (`1` = right, `-1` = left). Pipes move by `-d * SPEED` every frame, so flipping `d` makes the whole world scroll the other way. When `score % TURN_EVERY == 0`, the game flips `d` and draws the bird mirrored so it faces the new direction.

## 🔧 Customize it

Change these values at the top of `flappy_bird.py`:

| Setting | Default | Effect |
|---------|---------|--------|
| `SPEED` | `3.2` | How fast the bird flies |
| `TURN_EVERY` | `5` | Points before the bird turns around |
| `GRAVITY` | `0.45` | How fast the bird falls |
| `FLAP` | `-8.5` | How strong each flap is |
| `GAP` | `190` | Gap between pipes (smaller = harder) |
| `SPACING` | `280` | Distance between pipes |
| `AUTHOR` | `"Tirth Patel"` | Name shown in the game |

## 📁 Project structure

```
flappy-bird/
├── flappy_bird.py      # the game
├── requirements.txt    # pygame
├── README.md
├── LICENSE
└── .gitignore
```

## 👤 Author

**Tirth Patel**, Computer Science student at the University of Regina

- GitHub: [@pateltirth128](https://github.com/pateltirth128)
- LinkedIn: [tirth1228](https://www.linkedin.com/in/tirth1228)

## 📄 License

Released under the [MIT License](LICENSE). Feel free to use and modify it, but please keep the credit.
