#pragma once

#include "little8_conf.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
  #ifdef LITTLE8_BUILD
    #define L8_API __declspec(dllexport)
  #else
    #define L8_API __declspec(dllimport)
  #endif
#else
  #define L8_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---- Точка входа ----
// init   — вызывается один раз при старте (может быть NULL)
// update — логика, каждый кадр (обязателен)
// draw   — рисование, каждый кадр (обязателен)
L8_API void little8(void (*init)(void),
                    void (*update)(void),
                    void (*draw)(void));

// Очистить экран цветом (0..15)
L8_API void cls(uint8_t color);

// Поставить пиксель (0..15)
L8_API void pset(int x, int y, uint8_t color);

// Прочитать цвет пикселя (0..15). За границами — 0.
L8_API uint8_t pget(int x, int y);

// Печать строки. '\n' — перенос, lowercase → uppercase, неизвестное → '?'.
L8_API void print(const char *s, int x, int y, uint8_t color);


L8_API void line(int x0, int y0, int x1, int y1, uint8_t color);
L8_API void rect (int x, int y, int w, int h, uint8_t color);   // залитый
L8_API void rectb(int x, int y, int w, int h, uint8_t color);   // рамка
L8_API void circ (int x, int y, int r, uint8_t color);          // залитый
L8_API void circb(int x, int y, int r, uint8_t color);          // рамка


// Кнопка зажата сейчас (0=left 1=right 2=up 3=down 4=A 5=B)
L8_API bool btn(int id, int player);

// Кнопка нажата в этом кадре (edge, «только что»)
L8_API bool btnp(int id, int player);

#ifdef __cplusplus
}
#endif