#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../efi/api.h"
#include "../gfx/psf1font.h"
#include "../gfx/graphics.h"
//#include "../utils/bitmap.h"
//#include "../memory/memory.h"
#include "../memory/page_allocator.h"

typedef struct {
    Framebuffer* framebuffer;
    PSF1_FONT* font;
    EFI_MEMORY_DESCRIPTOR* memMap;
    uint64_t memMapSize;
    uint64_t memMapDescSize;
} BootInfo;
