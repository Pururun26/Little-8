#include "little8.h"
#include "little8_font3x4.h"
#include "raylib.h"

// Палитра 16 цветов. Игра знает только индексы.
static const uint32_t palette[16] = {
    0x1a1c2c, 0x5d275d, 0xb13e53, 0xef7d57,
    0xffcd75, 0xa7f070, 0x38b764, 0x257179,
    0x29366f, 0x3b5dc9, 0x41a6f6, 0x73eff7,
    0xf4f4f4, 0x94b0c2, 0x566c86, 0x333c57
};

static uint8_t  fb[L8_SCREEN_H][L8_SCREEN_W];
static uint32_t pixels[L8_SCREEN_H * L8_SCREEN_W];


// ---------- Внутреннее ----------
static void flush_fb(void) {
    for (int y = 0; y < L8_SCREEN_H; y++) {
        for (int x = 0; x < L8_SCREEN_W; x++) {
            uint32_t c = palette[fb[y][x]];
            uint8_t r = (c >> 16) & 0xFF;
            uint8_t g = (c >>  8) & 0xFF;
            uint8_t b =  c        & 0xFF;
            pixels[y * L8_SCREEN_W + x] =
                (uint32_t)r | ((uint32_t)g << 8) |
                ((uint32_t)b << 16) | 0xFF000000u;
        }
    }
}


// ---------- Точка входа ----------
void little8(void (*init)(void),
             void (*update)(void),
             void (*draw)(void)) {
    InitWindow(L8_SCREEN_W * L8_WINDOW_SCALE,
               L8_SCREEN_H * L8_WINDOW_SCALE, L8_NAME);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(L8_SCREEN_W, L8_SCREEN_H);   // не меньше 1×
    SetTargetFPS(L8_TARGET_FPS);

    Image img = GenImageColor(L8_SCREEN_W, L8_SCREEN_H, BLACK);
    Texture2D screen = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(screen, TEXTURE_FILTER_POINT);

    cls(0);
    if (init) init();

    while (!WindowShouldClose()) {
        if (update) update();
        if (draw)   draw();
        flush_fb();
        UpdateTexture(screen, pixels);

        BeginDrawing();
            ClearBackground(BLACK);

            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            int scale_x = sw / L8_SCREEN_W;
            int scale_y = sh / L8_SCREEN_H;
            int scale   = (scale_x < scale_y) ? scale_x : scale_y;
            if (scale < 1) scale = 1;

            int dw = L8_SCREEN_W * scale;
            int dh = L8_SCREEN_H * scale;
            int ox = (sw - dw) / 2;
            int oy = (sh - dh) / 2;

            DrawTexturePro(
                screen,
                (Rectangle){ 0, 0, L8_SCREEN_W, L8_SCREEN_H },
                (Rectangle){ (float)ox, (float)oy, (float)dw, (float)dh },
                (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadTexture(screen);
    CloseWindow();
}


void cls(uint8_t color) {
    for (int y = 0; y < L8_SCREEN_H; y++)
        for (int x = 0; x < L8_SCREEN_W; x++)
            fb[y][x] = color & 0x0F;
}

void pset(int x, int y, uint8_t color) {
    if (x < 0 || x >= L8_SCREEN_W || y < 0 || y >= L8_SCREEN_H) return;
    fb[y][x] = color & 0x0F;
}

uint8_t pget(int x, int y) {
    if (x < 0 || x >= L8_SCREEN_W || y < 0 || y >= L8_SCREEN_H) return 0;
    return fb[y][x];
}

void print(const char *s, int x, int y, uint8_t color) {
    if (!s) return;

    int cx = x, cy = y;

    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        if (*p == '\n') { cx = x; cy += FONT_CELL_H + 1; continue; }
        if (*p == '\r') continue;

        unsigned char u = *p;
        if (u >= 'a' && u <= 'z') u -= 32;

        int idx = (int)u - FONT_FIRST;
        if (idx < 0 || idx >= FONT_COUNT) idx = '?' - FONT_FIRST;

        int w = l8_font_w[idx];
        for (int row = 0; row < FONT_CELL_H; row++) {
            unsigned char bits = l8_font[idx][row];
            for (int col = 0; col < FONT_CELL_W; col++) {
                if ((bits >> (FONT_CELL_W - 1 - col)) & 1)
                    pset(cx + col, cy + row, color);
            }
        }
        cx += w + 1;
    }
}


bool btn(int id, int player) {
    switch (player) {
        case 0:  // P1: WASD + J/K
            switch (id) {
                case 0: return IsKeyDown(KEY_A);
                case 1: return IsKeyDown(KEY_D);
                case 2: return IsKeyDown(KEY_W);
                case 3: return IsKeyDown(KEY_S);
                case 4: return IsKeyDown(KEY_J);
                case 5: return IsKeyDown(KEY_K);
            }
            break;
        case 1:  // P2: стрелки + Numpad 4/5
            switch (id) {
                case 0: return IsKeyDown(KEY_LEFT);
                case 1: return IsKeyDown(KEY_RIGHT);
                case 2: return IsKeyDown(KEY_UP);
                case 3: return IsKeyDown(KEY_DOWN);
                case 4: return IsKeyDown(KEY_KP_4);
                case 5: return IsKeyDown(KEY_KP_5);
            }
            break;
    }
    return false;
}

bool btnp(int id, int player) {
    switch (player) {
        case 0:
            switch (id) {
                case 0: return IsKeyPressed(KEY_A);
                case 1: return IsKeyPressed(KEY_D);
                case 2: return IsKeyPressed(KEY_W);
                case 3: return IsKeyPressed(KEY_S);
                case 4: return IsKeyPressed(KEY_J);
                case 5: return IsKeyPressed(KEY_K);
            }
            break;
        case 1:
            switch (id) {
                case 0: return IsKeyPressed(KEY_LEFT);
                case 1: return IsKeyPressed(KEY_RIGHT);
                case 2: return IsKeyPressed(KEY_UP);
                case 3: return IsKeyPressed(KEY_DOWN);
                case 4: return IsKeyPressed(KEY_KP_4);
                case 5: return IsKeyPressed(KEY_KP_5);
            }
            break;
    }
    return false;
}