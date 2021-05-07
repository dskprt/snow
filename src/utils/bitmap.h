#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t* buffer;
    size_t size;
} Bitmap;

Bitmap BpCreate(uint8_t* buffer, size_t size);
bool BpGetValue(Bitmap bitmap, uint64_t index);
void BpSetValue(Bitmap bitmap, uint64_t index, bool value);
