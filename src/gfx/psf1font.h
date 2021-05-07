#pragma once
#include <stddef.h>
#include "../libc/string.h"

typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} PSF1_HEADER;

typedef struct {
    PSF1_HEADER* header;
    void* glyphBuffer;
} PSF1_FONT;

static inline size_t GetTextWidth(PSF1_FONT* font, const char* str) {
    return strlen(str) * (font->header->charsize / 2);
}
