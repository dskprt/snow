#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../efi/api.h"
#include "gfx/psf1font.hpp"
#include "gfx/graphics.hpp"
//#include "utils/bitmap.h"
//#include "memory/memory.h"
#include "memory/paging/page_frame_allocator.hpp"
#include "memory/paging/page_table_manager.hpp"
#include "memory/heap.hpp"
#include "gdt/gdt.hpp"
#include "idt/interrupts.hpp"
#include "idt/idt.hpp"
#include "io/ports.hpp"
#include "terminal/terminal.hpp"

struct BootInfo {
    Framebuffer* framebuffer;
    PSF1_FONT* font;
    EFI_MEMORY_DESCRIPTOR* memMap;
    uint64_t memMapSize;
    uint64_t memMapDescSize;
};

extern BootInfo* bootInfo;
