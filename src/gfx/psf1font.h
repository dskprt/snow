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

    inline size_t GetTextWidth(const char* str) {
        return strlen(str) * (header->charsize / 2);
    }
} PSF1_FONT;
