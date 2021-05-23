#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../kernel/memory/heap.hpp"

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* p);
void operator delete[](void* p);

void* malloc(size_t size);
void free(void* address);

static inline void memcpy(char* dest, char* src, int len) {
    for(int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

static inline void* memset(void* start, int value, size_t num) {
    unsigned char* p = (unsigned char*) start;
    unsigned char x = value & 0xff;

    while(num--) {
        *p++ = x;
    }

    return start;
}

static inline void* realloc(void* ptr, size_t oldSize, size_t newSize) { 
    void* newptr = malloc(oldSize);
    
    if(newptr == NULL) {
        return NULL;
    }

    memcpy((char*) newptr, (char*) ptr, newSize);
    free(ptr);

    return newptr;
}
