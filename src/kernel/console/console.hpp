#pragma once
#include "../gfx/graphics.hpp"
#include "../gfx/psf1font.hpp"
#include "../../libc/math.hpp"
#include "../../libc/mem.hpp"
#include "../kernel.hpp"

extern int _console_width;
extern int _console_height;
extern int _console_bufferWidth;
extern int _console_bufferHeight;

extern int _console_cursorX;
extern int _console_cursorY;

class Console {
public:
    static void Initialize(Framebuffer* framebuffer, PSF1_FONT* font);
    static void Write(char* str, int color);

    static inline void Write(char c, int color) {
        int index = _console_cursorY * _console_bufferWidth + _console_cursorX;

        buffer[index] = c;
        colorBuffer[index] = color;

        if(c == '\n') {
            _console_cursorX = 0;
            _console_cursorY++;
        } else {
            _console_cursorX++;

            if(_console_cursorX > _console_bufferWidth) {
                _console_cursorX = 0;
                _console_cursorY++;
            }
        }
    }

    static inline void Write(char c) {
        if(c == '\b') {
            Backspace();
            return;
        }
        
        Write(c, 0xFFFFFFFF);
    }

    static inline void Write(char* str) {
        Write(str, 0xFFFFFFFF);
    }

    static inline void WriteLine(char c, int color) {
        Write(c, color);
        Write('\n', 0);
    }

    static inline void WriteLine(char c) {
        WriteLine(c, 0xFFFFFFFF);
    }

    static inline void WriteLine(char* str, int color) {
        Write(str, color);
        Write('\n', 0);
    }

    static inline void WriteLine(char* str) {
        WriteLine(str, 0xFFFFFFFF);
    }

    static void Backspace();
    static void Flush();
private:
    static Framebuffer* framebuffer;
    static PSF1_FONT* font;

    static char* buffer;
    static int* colorBuffer;
};
