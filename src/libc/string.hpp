#pragma once
#include <limits.h>
#include <stddef.h>

int itoa(int value, char *sp, int radix);

static inline size_t strlen(const char* str) {
    const char* s;
    for(s = str; *s; ++s);

    return (s - str);
}
