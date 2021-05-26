#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../kernel/memory/heap.hpp"
#include "../kernel/memory/memory.hpp"

void* malloc(size_t size);
void* realloc(void* ptr, size_t oldSize, size_t newSize);
void free(void* address);

void memcpy(char* dest, char* src, int len);
void* memset(void* start, int value, size_t num);
