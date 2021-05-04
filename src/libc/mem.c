#include "mem.h"

void memcpy(char* dest, char* src, int len) {
    for(int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}
