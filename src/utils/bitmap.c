#include "bitmap.h"

Bitmap BpCreate(uint8_t* buffer, size_t size) {
    Bitmap bitmap;
    bitmap.buffer = buffer;
    bitmap.size = size;

    return bitmap;
}

bool BpGetValue(Bitmap bitmap, uint64_t index) {
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    if((bitmap.buffer[byteIndex] & bitIndexer) > 0) {
        return true;
    } else {
        return false;
    }
}

void BpSetValue(Bitmap bitmap, uint64_t index, bool value) {
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    bitmap.buffer[byteIndex] &= ~bitIndexer;

    if(value) {
        bitmap.buffer[byteIndex] |= bitIndexer;
    }
}
