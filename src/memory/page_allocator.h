#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../efi/api.h"
#include "../memory/memory.h"
#include "../utils/bitmap.h"

extern Bitmap bitmap;

void PgInitialize(EFI_MEMORY_DESCRIPTOR* map, size_t mapSize, size_t descSize);
void LockPage(void* address);
void LockPages(void* address, uint64_t count);
void FreePage(void* address);
void FreePages(void* address, uint64_t count);
void* RequestPage();
uint64_t GetFreeMemory();
uint64_t GetUsedMemory();
uint64_t GetReservedMemory();
