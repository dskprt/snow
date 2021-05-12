#include "mem.hpp"

void memcpy(char* dest, char* src, int len) {
    for(int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

void* memset(void* start, int value, size_t num) {
    unsigned char* p = (unsigned char*) start;
    unsigned char x = value & 0xff;

    while(num--) {
        *p++ = x;
    }

    return start;
}
