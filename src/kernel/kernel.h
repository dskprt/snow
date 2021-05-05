#include <stdint.h>
#include "../gfx/psf1font.h"
#include "../gfx/graphics.h"
#include "../efi/memory.h"

typedef struct {
    Framebuffer* framebuffer;
    PSF1_FONT* font;
    void* memMap;
    uint64_t memMapSize;
    uint64_t memMapDescriptorSize;
} BootInfo;
