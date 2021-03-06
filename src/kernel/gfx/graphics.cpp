#include "graphics.hpp"

void Graphics::DrawChar(Framebuffer* framebuffer, PSF1_FONT* font, char chr, int x, int y, unsigned int color) {
    unsigned int* bufPtr = framebuffer->Address;
    char* fontPtr = (char*) font->glyphBuffer + (chr * font->header->charsize);

    for(unsigned long _y = y; _y < y + 16; _y++) {
        for(unsigned long _x = x; _x < x + 8; _x++) {
            if((*fontPtr & (0b10000000 >> (_x - x))) > 0) {
                *(unsigned int*)(bufPtr + _x + (_y * framebuffer->PixelsPerScanLine)) = color;
            }
        }

        fontPtr++;
    }
}

void Graphics::DrawString(Framebuffer* framebuffer, PSF1_FONT* font, char* text, int x, int y, unsigned int color) {
    char* chr = text;

    int cursorX = x;
    int cursorY = y;

    while(*chr != 0) {
        if(*chr == '\n') {
            cursorX = 0;
            cursorY += font->header->charsize;
            continue;
        }

        DrawChar(framebuffer, font, *chr, cursorX, cursorY, color);
        cursorX += font->header->charsize / 2;

        if(cursorX + (font->header->charsize / 2) > framebuffer->Width) {
            cursorX = 0;
            cursorY += font->header->charsize;
        }

        chr++;
    }
}

void Graphics::DrawPixel(Framebuffer* framebuffer, int x, int y, unsigned int color) {
   *((unsigned int*) (framebuffer->Address
    + ((framebuffer->BytesPerPixel * framebuffer->PixelsPerScanLine) * y)
    + (framebuffer->BytesPerPixel * x))) = color;
}
