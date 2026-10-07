"""
Flappy Bird - Left to Right (and back!)
Author: Tirth Patel

The bird starts on the LEFT and flies RIGHT. Every 5 points it turns around
and flies the other way. Everything is drawn with pygame, so no images needed.

Controls: SPACE / UP / left click = flap (start, restart)   P = pause   ESC = quit
"""
import math
import os
import random
import sys

import pygame

AUTHOR = "Tirth Patel"
W, H, FPS = 600, 800, 60
FLOOR = H - 100
GRAVITY, FLAP, MAX_FALL = 0.45, -8.5, 10
SPEED, TURN_EVERY = 3.2, 5            # speed (px/frame), turn around every N points
PIPE_W, GAP, SPACING = 80, 190, 280
R = 17                                 # bird radius
HS_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "highscore.txt")
C = dict(sky1=(78, 192, 202), sky2=(180, 235, 240), pipe=(94, 190, 60), dark=(58, 130, 36),
         light=(150, 225, 110), sand=(222, 216, 149), grass=(116, 191, 46), stripe=(200, 190, 120),
         bird=(250, 200, 40), beak=(240, 120, 30), white=(255, 255, 255), black=(0, 0, 0),
         cloud=(240, 250, 252))


class Bird:
    def __init__(self):
        self.x, self.y, self.vy, self.flap_t = 40.0, H / 2 - 60, 0.0, 0   # starts at the left edge

    def flap(self):
        self.vy, self.flap_t = FLAP, 10

    def rect(self):
        return pygame.Rect(self.x - R + 3, self.y - R + 3, 2 * R - 6, 2 * R - 6)

    def draw(self, surf, d, tick):
        n = 2 * R + 12
        c = n // 2
        img = pygame.Surface((n, n), pygame.SRCALPHA)
        wing = pygame.Rect(c - 16, c - 8 if self.flap_t else c - 2 + int(3 * math.sin(tick * 0.3)), 16, 10)
        beak = [(c + 10, c + 1), (c + 24, c + 5), (c + 10, c + 10)]
        parts = [("circle", C["bird"], (c, c), R, 0), ("circle", C["black"], (c, c), R, 2),
                 ("ellipse", C["white"], wing, 0), ("ellipse", C["black"], wing, 2),
                 ("circle", C["white"], (c + 7, c - 6), 7, 0), ("circle", C["black"], (c + 7, c - 6), 7, 2),
                 ("circle", C["black"], (c + 9, c - 6), 3, 0),
                 ("polygon", C["beak"], beak, 0), ("polygon", C["black"], beak, 2)]
        for shape, colour, *args in parts:
            getattr(pygame.draw, shape)(img, colour, *args)
        img = pygame.transform.flip(img, d < 0, False)                  # face the way we fly
        img = pygame.transform.rotate(img, max(-70, min(25, -self.vy * 4)) * d)
        surf.blit(img, img.get_rect(center=(self.x, self.y)))


class Pipe:
    def __init__(self, x):
        self.x, self.passed = x, False
        self.gy = random.randint(80 + GAP // 2, FLOOR - 80 - GAP // 2)   # centre of the gap

    def rects(self):
        return [pygame.Rect(self.x, 0, PIPE_W, self.gy - GAP // 2),
                pygame.Rect(self.x, self.gy + GAP // 2, PIPE_W, FLOOR - self.gy - GAP // 2)]

    def draw(self, surf):
        for i, r in enumerate(self.rects()):
            cap = pygame.Rect(r.x - 6, r.bottom - 30 if i == 0 else r.y, PIPE_W + 12, 30)
            for part in (r, cap):
                pygame.draw.rect(surf, C["pipe"], part)
                pygame.draw.rect(surf, C["light"], (part.x + 8, part.y, 10, part.h))
                pygame.draw.rect(surf, C["dark"], part, 3)


class Game:
    def __init__(self):
        pygame.init()
        pygame.display.set_caption(f"Flappy Bird | by {AUTHOR}")
        self.screen, self.clock = pygame.display.set_mode((W, H)), pygame.time.Clock()
        self.fonts = [pygame.font.SysFont("arial", size, bold=b) for size, b in ((64, 1), (30, 1), (22, 0))]
        self.sky = pygame.Surface((W, FLOOR))
        for y in range(FLOOR):
            colour = [a + (b - a) * y / FLOOR for a, b in zip(C["sky1"], C["sky2"])]
            pygame.draw.line(self.sky, colour, (0, y), (W, y))
        try:
            self.best = int(open(HS_FILE).read())
        except (OSError, ValueError):
            self.best = 0
        self.reset()

    def reset(self):
        self.bird, self.pipes = Bird(), [Pipe(450 + i * SPACING) for i in range(3)]
        self.d, self.score, self.scroll, self.tick, self.turn_t = 1, 0, 0.0, 0, 0   # d: 1 = right, -1 = left
        self.state, self.paused, self.turn_pending = "ready", False, False

    def behind(self, p):          # has the bird fully passed this pipe (in the current direction)?
        return (self.bird.x - p.x - PIPE_W / 2) * self.d > PIPE_W / 2

    def clear_of_pipes(self):    # is the bird about halfway between two pipes?
        need = (SPACING - PIPE_W) / 2 - R - 4
        return all(abs(p.x + PIPE_W / 2 - self.bird.x) - PIPE_W / 2 - R >= need for p in self.pipes)

    def turn(self):
        self.d, self.turn_t, self.turn_pending = -self.d, 90, False
        for p in self.pipes:
            p.passed = self.behind(p)

    def die(self):
        self.state = "dead"
        if self.score > self.best:
            self.best = self.score
            with open(HS_FILE, "w") as f:
                f.write(str(self.best))

    def events(self):
        for e in pygame.event.get():
            key = e.key if e.type == pygame.KEYDOWN else None
            if e.type == pygame.QUIT or key == pygame.K_ESCAPE:
                return False
            if key == pygame.K_p and self.state == "playing":
                self.paused = not self.paused
            if key in (pygame.K_SPACE, pygame.K_UP) or (e.type == pygame.MOUSEBUTTONDOWN and e.button == 1):
                if self.state == "dead":
                    if self.bird.y >= FLOOR - R:
                        self.reset()
                elif not self.paused:
                    self.state = "playing"
                    self.bird.flap()
        return True

    def update(self):
        b, self.tick = self.bird, self.tick + 1
        if self.paused:
            return
        b.flap_t, self.turn_t = max(0, b.flap_t - 1), max(0, self.turn_t - 1)
        if self.state == "ready":
            b.y = H / 2 - 60 + 8 * math.sin(self.tick * 0.08)
            return
        dead = self.state == "dead"
        b.vy = min(b.vy + GRAVITY, MAX_FALL + 4 * dead)
        b.y = min(b.y + b.vy, FLOOR - R)
        if dead:
            return
        # glide to 1/3 of the screen from the side we came from, while the world scrolls past
        b.x += max(-SPEED, min(SPEED, W / 2 - self.d * W / 6 - b.x))
        self.scroll += self.d * SPEED
        for p in self.pipes:
            p.x -= self.d * SPEED
        front = max(self.pipes, key=lambda p: p.x * self.d)
        if (front.x + PIPE_W / 2 - W / 2) * self.d < W / 2 + PIPE_W:
            self.pipes.append(Pipe(front.x + self.d * SPACING))
        self.pipes = [p for p in self.pipes if (W / 2 - p.x - PIPE_W / 2) * self.d < W / 2 + 2 * SPACING]
        for p in self.pipes:
            if any(b.rect().colliderect(r) for r in p.rects()):
                return self.die()
            if not p.passed and self.behind(p):
                p.passed, self.score = True, self.score + 1
                if self.score % TURN_EVERY == 0:
                    # don't turn while still inside this pipe: wait until the bird is
                    # halfway to the next one, otherwise it gets stuck in the pipe and dies
                    self.turn_pending, self.turn_t = True, 90
        if self.turn_pending and self.clear_of_pipes():
            self.turn()
        if b.y >= FLOOR - R:
            self.die()
        elif b.y < R:
            b.y, b.vy = R, 0

    def draw(self):
        sc, off = self.screen, self.scroll * 0.3
        sc.blit(self.sky, (0, 0))
        for i in range(int(off // 260) - 1, int(off // 260) + 4):                   # parallax clouds
            rnd = random.Random(i)
            x, y = i * 260 - off + rnd.randint(0, 80), 60 + rnd.randint(0, 220)
            for dx, dy, r in ((0, 0, 28), (30, -12, 34), (62, 0, 26), (30, 8, 26)):
                pygame.draw.circle(sc, C["cloud"], (x + dx, y + dy), r)
        for p in self.pipes:
            p.draw(sc)
        for colour, rect in ((C["sand"], (0, FLOOR, W, 100)), (C["grass"], (0, FLOOR, W, 16))):
            pygame.draw.rect(sc, colour, rect)
        for x in range(-40 - int(self.scroll) % 40, W + 40, 40):                   # moving ground
            pygame.draw.polygon(sc, C["stripe"], [(x, FLOOR + 16), (x + 20, FLOOR + 16), (x + 10, FLOOR + 30), (x - 10, FLOOR + 30)])
        self.bird.draw(sc, self.d, self.tick)

        if self.state == "ready":
            texts = [("FLAPPY BIRD", 0, 180), ("Fly from left to right!", 1, 250), (f"Made by {AUTHOR}", 2, 290),
                     ("SPACE / Click to flap", 2, 520), (f"Turns around every {TURN_EVERY} points", 2, 555),
                     (f"Best: {self.best}", 2, 590)]
        else:
            texts = [(str(self.score), 0, 70)]
        if self.turn_t:
            texts.append(("TURN AROUND!", 1, 140))
        if self.state == "dead":
            panel = pygame.Surface((380, 220), pygame.SRCALPHA)
            panel.fill((0, 0, 0, 140))
            sc.blit(panel, panel.get_rect(center=(W / 2, 330)))
            texts += [("GAME OVER", 0, 270), (f"Score: {self.score}   Best: {self.best}", 1, 340), (f"Made by {AUTHOR}", 2, H - 40)]
            texts += [("SPACE / Click to restart", 2, 395)] * (self.bird.y >= FLOOR - R)
        if self.paused:
            texts.append(("PAUSED", 0, H / 2))
        for text, f, y in texts:                                                    # text with shadow
            for colour, o in ((C["black"], 3), (C["white"], 0)):
                img = self.fonts[f].render(text, True, colour)
                sc.blit(img, img.get_rect(center=(W / 2 + o, y + o)))
        pygame.display.flip()

    def run(self):
        while self.events():
            self.update()
            self.draw()
            self.clock.tick(FPS)
        pygame.quit()


if __name__ == "__main__":
    Game().run()
    sys.exit()