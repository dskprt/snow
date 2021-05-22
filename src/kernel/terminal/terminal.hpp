#pragma once
#include "../gfx/graphics.hpp"
#include <stdint.h>

class Terminal {
public:
    static PSF1_FONT* font;

    static void Initialize(Framebuffer* _framebuffer, PSF1_FONT* _font);
    static void PrintChar(char c);
    static void PrintChar(char c, unsigned int color);
    static void Print(char* str);
    static void Print(char* str, unsigned int color);
    static void PrintLn(char* str);
    static void PrintLn(char* str, unsigned int color);
    static void Backspace();
private:
    static int16_t cursorX;
    static int16_t cursorY;

    static Framebuffer* framebuffer;
};
