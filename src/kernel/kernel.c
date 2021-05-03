#include "../gfx/psf1font.h"
#include "../gfx/graphics.h"

void kmain(Framebuffer* framebuffer, PSF1_FONT* font) {
    GfxDrawString(framebuffer, font, "chungus the big", 50, 50, 0xFFFFFFFF);

    while(1);
}
