#include "psf1font.h"
#include "graphics.h"

void GfxDrawChar(Framebuffer* framebuffer, PSF1_FONT* font, char chr, int x, int y, unsigned int color) {
    unsigned int* bufPtr = framebuffer->Address;
    char* fontPtr = font->glyphBuffer + (chr * font->header->charsize);

    for(unsigned long _y = y; y < y + 16; _y++) {
        for(unsigned long _x = x; x < x + 8; _x++) {
            if((*fontPtr & (0b10000000 >> (_x - x))) > 0) {
                *(unsigned int*)(bufPtr + _x + (_y * framebuffer->PixelsPerScanLine)) = color;
            }
        }

        fontPtr++;
    }
}

void GfxDrawString(Framebuffer* framebuffer, PSF1_FONT* font, char* text, int x, int y, unsigned int color) {
    char* chr = text;

    int cursorX = x;
    int cursorY = y;

    while(*chr != 0) {
        GfxDrawChar(framebuffer, font, *chr, x, y, color);
        cursorX += 8 + 2;

        if(cursorX + 8 + 2 > framebuffer->Width) {
            cursorX = 0;
            cursorY += 16 + 2;
        }

        chr++;
    }
}

void GfxDrawPixel(Framebuffer* framebuffer, int x, int y, unsigned int color) {
   *((unsigned int*) (framebuffer->Address
    + ((framebuffer->BytesPerPixel * framebuffer->PixelsPerScanLine) * y)
    + (framebuffer->BytesPerPixel * x))) = color;
}
