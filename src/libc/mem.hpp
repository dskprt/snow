#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../kernel/memory/heap.hpp"

void memcpy(char* dest, char* src, int len);
void* memset(void* start, int value, size_t num);
void* malloc(size_t size);
void free(void* address);
