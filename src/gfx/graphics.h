typedef struct {
    unsigned int* Address;
    unsigned long long BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelsPerScanLine;
    unsigned int BytesPerPixel;
} Framebuffer;

void GfxDrawChar(Framebuffer* framebuffer, PSF1_FONT* font, char chr, int x, int y, unsigned int color);
void GfxDrawString(Framebuffer* framebuffer, PSF1_FONT* font, char* text, int x, int y, unsigned int color);
void GfxDrawPixel(Framebuffer* framebuffer, int x, int y, unsigned int color);
