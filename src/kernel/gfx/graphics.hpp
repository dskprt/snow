#pragma once
#include "psf1font.hpp"

struct Framebuffer {
    unsigned int* Address;
    unsigned long long BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelsPerScanLine;
    unsigned int BytesPerPixel;
};

class Graphics {
public:
    static void DrawChar(Framebuffer* framebuffer, PSF1_FONT* font, char chr, int x, int y, unsigned int color);
    static void DrawString(Framebuffer* framebuffer, PSF1_FONT* font, char* text, int x, int y, unsigned int color);
    static void DrawPixel(Framebuffer* framebuffer, int x, int y, unsigned int color);
};
