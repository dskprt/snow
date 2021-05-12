#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct Bitmap {
    uint8_t* buffer;
    size_t size;

    bool Get(uint64_t index) {
        if(index > size * 8) return false;

        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;

        if((buffer[byteIndex] & bitIndexer) > 0) {
            return true;
        } else {
            return false;
        }
    }

    bool Set(uint64_t index, bool value) {
        if(index > size * 8) return false;

        uint64_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        uint8_t bitIndexer = 0b10000000 >> bitIndex;

        buffer[byteIndex] &= ~bitIndexer;

        if(value) {
            buffer[byteIndex] |= bitIndexer;
        }

        return true;
    }
};
