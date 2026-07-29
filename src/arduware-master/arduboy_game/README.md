<img src="arduware_title.png" width="768">

# ArduWare

A fast-paced Arduboy minigame collection in the style of WarioWare. You'll be thrown into a rapid sequence of short, frantic minigames — each lasting only a few seconds. React fast and do well!

---

## Navigation

| Screen | Controls |
|---|---|
| **Splash screen** | A or B — start |
| **Main menu** | ↑ / ↓ — navigate, A or B — select |
| **Settings** | A — toggle volume, B — toggle game mode, ← — back |

---

## Game Modes

### 🟢 Survival Mode *(default)*

Instead of tracking a score, you start with a **full health bar** displayed at the top of the screen. After each minigame, your health is reduced based on how much of the maximum possible points you *didn't* earn.

- **Miss 0%** of a game's points → lose **0%** health
- **Miss 100%** of a game's points → lose **50%** health (penalties are halved to keep things fair)
- The game ends when your health reaches zero.

Toggle between modes in the **Settings** menu.

### 🔵 Score Mode

The classic mode. Every minigame awards points and the game ends after playing through all the minigames.

---

## Minigames

### 🎯 Buttons!
Arrow icons fall from the top of the screen. Press the matching button before each arrow passes the target line at the bottom.

| Action | Control |
|---|---|
| Match arrow / button | ← ↑ ↓ → A B |

- **10 arrows** per round, **10 pts** each
- Misses reduce your score slightly

---

### 🧠 Repeat! (Simon)
Watch a pattern of 5 buttons flash on screen, then repeat it back in order. One wrong press and the round ends immediately.

| Action | Control |
|---|---|
| Input buttons | ← ↑ ↓ → A B |

- **5 steps**, **10 pts** per correct press
- A single wrong press ends the game — partial credit applies in Survival Mode

---

### 🔦 Chase! (Spotlight)
A smiley face hides in the dark. Move your spotlight to find and stay on top of it. The smiley tries to escape!

| Action | Control |
|---|---|
| Move spotlight | ← ↑ ↓ → |

- Earn **10 pts every 0.5 seconds** while overlapping
- Round lasts **6 seconds**. Max 120 pts.
- RGB LED turns **green** while overlapping, **red** when not

---

### 🌡️ Smash! (Make It Hot)
A thermometer drains constantly. Smash A to fill it up and cross each of the 4 section lines before the timer runs out.

| Action | Control |
|---|---|
| Fill thermometer | A (rapid press) |

- **4 sections**, **10 pts** each = 40 pts max
- Round lasts **6 seconds**

---

### ☕ Catch! (Marshmallow Drop)
6 marshmallows fall from the sky at random intervals and speeds. Move your mug to catch them before they hit the ground!

| Action | Control |
|---|---|
| Move mug | ← → |

- **10 pts** per caught marshmallow, **60 pts** max
- Mug wraps around the edges of the screen

---

### 🦆 Hunt! (Duck Hunt)
4 ducks zip around the screen. Move your crosshair and shoot them all before time runs out. Ducks actively evade you!

| Action | Control |
|---|---|
| Move crosshair | ← ↑ ↓ → |
| Shoot | A or B |

- **10 pts** per duck, **40 pts** max

---

### 🚀 Dodge! (Space Dodge)
You start with 60 points and a field of asteroids. Every collision costs you 10 points — stay alive and keep your score!

| Action | Control |
|---|---|
| Move spaceship | ← ↑ ↓ → |

- Starts at **60 pts**, **-10** per asteroid hit
- Round ends after all asteroids pass

---

### 🔐 Pick! (Lockpick)
You start with 100 points that tick down every second. Rotate the pin to find the hidden sweet spot in the lock cylinder before you run out of points!

| Action | Control |
|---|---|
| Rotate pin | ← → |
| Attempt turn | A |

- Starts at **100 pts**, **-10/second**
- Win before too much time passes to keep a high score

---

### 🎨 Colors! (Color Grid)
A 4×4 grid of squares is shown. The RGB LED tells you the color of the square your cursor is on. Find and tap all 6 squares matching the target color named at the top!

| Action | Control |
|---|---|
| Move cursor | ← ↑ ↓ → |
| Select square | A or B |

- **10 pts** per correct square, **-10 pts** per wrong square
- **6 target squares**, **60 pts** max
- Round lasts **8 seconds**

---

### 🚦 Run! (Red Light, Green Light)
Move across the screen while the light is **green**. Freeze completely when it turns **red** — or you'll be caught!

| Action | Control |
|---|---|
| Move right | A or B (hold) |

- **Win** (reach the finish line) = **100 pts**
- **Fail** (move on red) = partial points based on distance traveled
- Timeout = partial points based on distance
- Round lasts up to **12 seconds**

---

### 🏃 Jump! (Hurdles)
Run automatically and navigate a series of obstacles. Jump over low hurdles, duck under high barriers!

| Action | Control |
|---|---|
| Jump | A or ↑ |
| Duck | B or ↓ |

- **+10 pts every 2 seconds** you survive, **50 pts** max
- One collision ends the round

---

## Credits

Built with [Arduboy2](https://github.com/MLXXXp/Arduboy2) and [ArduboyTones](https://github.com/MLXXXp/ArduboyTones).
