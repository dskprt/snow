#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../efi/api.h"
#include "../memory/memory.h"
#include "../utils/bitmap.h"



class PageFrameAllocator {
public:
    Bitmap bitmap;

    void Initialize(EFI_MEMORY_DESCRIPTOR* map, size_t mapSize, size_t descSize);
    void LockPage(void* address);
    void LockPages(void* address, uint64_t count);
    void FreePage(void* address);
    void FreePages(void* address, uint64_t count);
    void* RequestPage();
    uint64_t GetFreeMemory();
    uint64_t GetUsedMemory();
    uint64_t GetReservedMemory();
private:
    uint64_t freeMemory = 0;
    uint64_t usedMemory = 0;
    uint64_t reservedMemory = 0;
    bool initialized = false;

    void InitializeBitmap(size_t size, void* bufferAddress);
    void ReservePage(void* address);
    void ReservePages(void* address, uint64_t count);
    void ReleasePage(void* address);
    void ReleasePages(void* address, uint64_t count);
};
