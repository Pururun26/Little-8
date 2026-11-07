# Little-8

A small fantasy console framework for C.

Write one `game()` function — Little-8 handles the window, input, sound, and rendering loop.

```c
#include "little8.h"

static void game(void) {
    cls(0);
    if (btn(0, 0)) tone(440, 4, 50, 0);
    oval(60, 60, 8, 8, 8, true);
}

int main(void) {
    little8(game);
    return 0;
}
```

## Specs

- **Screen:** 128×128, 16-color palette (Sweetie 16)
- **Sprites:** 256 slots, any size up to 128×128
- **Save slots:** 64 × int64 (512 bytes on disk)
- **Audio:** 4 channels (pulse 12.5%, pulse 25%, triangle, noise)
- **Input:** 2 players × 6 buttons
- **Code size:** no limit

## Build

Put `little8.h`, `little8.dll`, and `liblittle8.dll.a` next to your `game.c`:

```
gcc game.c -I. -L. -llittle8 -o game.exe
```

Or with CMake:

```cmake
add_executable(game game.c)
target_link_libraries(game PRIVATE little8)
```

`little8.dll` must sit next to `game.exe` at runtime.

## API

### Screen

```c
void    cls(uint8_t color);                            // clear to color 0..15
void    pset(int x, int y, uint8_t color);             // set pixel
uint8_t pget(int x, int y);                            // get pixel (0 outside)
```

### Drawing

```c
void line(int x0, int y0, int x1, int y1, uint8_t color);
void rect(int x, int y, int w, int h, uint8_t color, bool filled);
void oval(int x, int y, int w, int h, uint8_t color, bool filled);   // circle: w == h
```

### Sprites

```c
void sprdata(int id, const uint8_t *data, int w, int h);                       // load
void spr(int id, int x, int y, bool flip_x, bool flip_y, int colorkey);        // draw
```

`data` is `w*h` bytes, row-major, values `0..15`.

`colorkey` — which color is **not** drawn:

| value | behavior |
|---|---|
| `-1` | all pixels drawn (no transparency) |
| `0..15` | pixels of this color are skipped |

Example:

```c
static const uint8_t heart[64] = {
    0,2,2,0, 0,2,2,0,
    2,0,0,2, 2,0,0,2,
    2,0,0,0, 0,0,0,2,
    2,0,0,0, 0,0,0,2,
    0,2,0,0, 0,0,2,0,
    0,0,2,0, 0,2,0,0,
    0,0,0,2, 2,0,0,0,
    0,0,0,0, 0,0,0,0,
};
sprdata(1, heart, 8, 8);

spr(1, 10, 10, false, false, 0);    // color 0 transparent (PICO-8 style)
spr(1, 20, 10, true,  false, 0);    // flipped horizontally
spr(1, 30, 10, false, false, -1);   // all pixels visible (solid)
```

### Text

```c
void print(const char *s, int x, int y, uint8_t color);
```

Built-in 3×4 proportional font. Uppercase only. `\n` starts a new line.

### Input

```c
bool btn (int id, int player);   // held
bool btnp(int id, int player);   // pressed this frame
```

| id | P1 | P2 |
|---|---|---|
| 0 | A | ← |
| 1 | D | → |
| 2 | W | ↑ |
| 3 | S | ↓ |
| 4 | J | Num 4 |
| 5 | K | Num 5 |

### Audio

```c
void tone(int frequency, int duration, int volume, int channel);
```

| channel | wave |
|---|---|
| 0 | pulse 12.5% |
| 1 | pulse 25% |
| 2 | triangle |
| 3 | noise (frequency ignored) |

`frequency` in Hz, `duration` in frames (1/60 s), `volume` in 0–100.  
`tone(0, 0, 0, ch)` stops the channel.

### Save / Load

```c
void    dset(int slot, int64_t value);    // 0..63
int64_t dget(int slot);
```

Written to `save/storage.bin` next to the executable. Each `dset` writes to disk immediately (skipped if value unchanged).

### Entry point

```c
void little8(void (*callback)(void));
```

Opens a window, runs a 60 FPS loop, calls `callback` each frame. Returns when the window is closed. `callback == NULL` → black screen.

## Palette

| # | Hex | # | Hex |
|---|---|---|---|
| 0 | `1a1c2c` | 8 | `29366f` |
| 1 | `5d275d` | 9 | `3b5dc9` |
| 2 | `b13e53` | 10 | `41a6f6` |
| 3 | `ef7d57` | 11 | `73eff7` |
| 4 | `ffcd75` | 12 | `f4f4f4` |
| 5 | `a7f070` | 13 | `94b0c2` |
| 6 | `38b764` | 14 | `566c86` |
| 7 | `257179` | 15 | `333c57` |
```