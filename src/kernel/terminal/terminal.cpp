#include "terminal.hpp"

int16_t cursorX = 0;
int16_t cursorY = 0;

Framebuffer* framebuffer;
PSF1_FONT* font;

void Initialize(Framebuffer* _framebuffer, PSF1_FONT* _font) {
    framebuffer = _framebuffer;
    font = _font;
}

void Print(char c) {
    Print(c, 0xFFFFFFFF);
}

void Print(char c, unsigned int color) {
    if(c == '\n') {
        cursorX = 0;
        cursorY += font->header->charsize;

        return;
    }

    Graphics::DrawChar(framebuffer, font, c, cursorX, cursorY, color);
    cursorX += font->header->charsize / 2;

    if(cursorX + (font->header->charsize / 2) > framebuffer->Width) {
        cursorX = 0;
        cursorY += font->header->charsize;
    }
}

void Print(char* str) {
    Print(str, 0xFFFFFFFF);
}

void Print(char* str, unsigned int color) {
    char* c = text;

    while(*c != 0) {
        if(*c == '\n') {
            cursorX = 0;
            cursorY += font->header->charsize;

            continue;
        }

        Graphics::DrawChar(framebuffer, font, *c, cursorX, cursorY, color);
        cursorX += font->header->charsize / 2;

        if(cursorX + (font->header->charsize / 2) > framebuffer->Width) {
            cursorX = 0;
            cursorY += font->header->charsize;
        }

        c++;
    }
}

void PrintLn(char* str) {
    PrintLn(str, 0xFFFFFFFF);
}

void PrintLn(char* str, unsigned int color) {
    Print(str, color);

    cursorX = 0;
    cursorY += font->header->charsize;
}

void Backspace() {
    if(cursorX - (font->header->charsize / 2) < 0) {
        cursorX = 0;
        cursorY -= font->header->charsize;
    } else {
        cursorX -= font->header->charsize / 2;
    }

    Graphics::DrawChar(framebuffer, font, '█', cursorX, cursorY, 0x00000000);
}
