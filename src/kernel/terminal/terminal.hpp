#pragma once
#include "../gfx/graphics.hpp"
#include <stdint.h>

class Terminal {
public:
    PSF1_FONT* font;

    static void Initialize(Framebuffer* _framebuffer, PSF1_FONT* _font);
    static void Print(char c);
    static void Print(char c, unsigned int color);
    static void Print(char* str);
    static void Print(char* str, unsigned int color);
    static void PrintLn(char* str);
    static void PrintLn(char* str, unsigned int color);
    static void Backspace();
private:
    int16_t cursorX;
    int16_t cursorY;

    Framebuffer* framebuffer;
};
