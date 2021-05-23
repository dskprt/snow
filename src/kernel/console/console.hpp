#pragma once
#include "../gfx/graphics.hpp"
#include "../gfx/psf1font.hpp"
#include "../../libc/math.hpp"
#include "../../libc/mem.hpp"
#include "../kernel.hpp"

class Console {
public:
    static int width;
    static int height;
    static int bufferWidth;
    static int bufferHeight;

    static int cursorX;
    static int cursorY;

    static void Initialize(Framebuffer* framebuffer, PSF1_FONT* font);
    static void Write(char* str, int color);

    static inline void Write(char c, int color) {
        int index = cursorY * bufferWidth + cursorX;

        buffer[index] = c;
        colorBuffer[index] = color;
    }

    static inline void Write(char c) {
        Write(c, 0xFFFFFF00);
    }

    static inline void Write(char* str) {
        Write(str, 0xFFFFFF00);
    }

    static inline void WriteLine(char c, int color) {
        Write(c, color);
        Write('\n', 0);
    }

    static inline void WriteLine(char c) {
        WriteLine(c, 0xFFFFFF00);
    }

    static inline void WriteLine(char* str, int color) {
        Write(str, color);
        Write('\n', 0);
    }

    static inline void WriteLine(char* str) {
        WriteLine(str, 0xFFFFFF00);
    }

    static void Backspace();
    static void Flush();
private:
    static Framebuffer* framebuffer;
    static PSF1_FONT* font;

    static char* buffer;
    static int* colorBuffer;
};
