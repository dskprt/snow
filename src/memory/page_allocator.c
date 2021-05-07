#include "page_allocator.h"

Bitmap bitmap;

static uint64_t freeMemory = 0;
static uint64_t usedMemory = 0;
static uint64_t reservedMemory = 0;
static bool initialized = false;

static void InitializeBitmap(size_t size, void* bufferAddress) {
    bitmap = BpCreate((uint8_t*) bufferAddress, size);

    for(int i = 0; i < size; i++) {
        *(uint8_t*) (bitmap.buffer + i) = 0;
    }
}

static void ReservePage(void* address) {
    uint64_t index = (uint64_t) address / PAGE_SIZE;
    if(BpGetValue(bitmap, index)) return;

    BpSetValue(bitmap, index, true);
    freeMemory -= PAGE_SIZE;
    reservedMemory += PAGE_SIZE;
}

static void ReservePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        ReservePage((void*) ((uint64_t) address + (i * PAGE_SIZE)));
    }
}

static void ReleasePage(void* address) {
    uint64_t index = (uint64_t) address / PAGE_SIZE;
    if(!BpGetValue(bitmap, index)) return;

    BpSetValue(bitmap, index, false);
    freeMemory += PAGE_SIZE;
    reservedMemory -= PAGE_SIZE;
}

static void ReleasePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        ReleasePage((void*) ((uint64_t) address + (i * PAGE_SIZE)));
    }
}

void PgInitialize(EFI_MEMORY_DESCRIPTOR* map, size_t mapSize, size_t descSize) {
    if(initialized) return;

    uint64_t entries = mapSize / descSize;

    void* largestFreeMemorySegment = NULL;
    size_t largestFreeMemorySegmentSize = 0;

    for(int i = 0; i < entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) map + (i * descSize));

        if(desc->type == 7) { // EfiConventionalMemory
            if(desc->numPages * PAGE_SIZE > largestFreeMemorySegmentSize) {
                largestFreeMemorySegment = desc->physAddr;
                largestFreeMemorySegmentSize = desc->numPages * PAGE_SIZE;
            }
        }
    }

    uint64_t totalMemory = GetTotalMemory(map, entries, descSize);
    freeMemory = totalMemory;

    uint64_t bitmapSize = ((totalMemory / PAGE_SIZE) / 8) + 1;

    InitializeBitmap(bitmapSize, largestFreeMemorySegment);
    LockPages(&bitmap, (bitmap.size / PAGE_SIZE) + 1);

    for(int i = 0; i < entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) map + (i * descSize));

        if(desc->type != 7) { // not EfiConventionalMemory
            ReservePages(desc->physAddr, desc->numPages);
        }
    }

    initialized = true;
}

void LockPage(void* address) {
    uint64_t index = (uint64_t) address / PAGE_SIZE;
    if(BpGetValue(bitmap, index)) return;

    BpSetValue(bitmap, index, false);
    freeMemory -= PAGE_SIZE;
    usedMemory += PAGE_SIZE;
}

void LockPages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        LockPage((void*) ((uint64_t) address + (i * PAGE_SIZE)));
    }
}

void FreePage(void* address) {
    uint64_t index = (uint64_t) address / PAGE_SIZE;
    if(!BpGetValue(bitmap, index)) return;

    BpSetValue(bitmap, index, false);
    freeMemory += PAGE_SIZE;
    usedMemory -= PAGE_SIZE;
}

void FreePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        FreePage((void*) ((uint64_t) address + (i * PAGE_SIZE)));
    }
}

void* RequestPage() {
    for(uint64_t i = 0; i < bitmap.size * 8; i++) {
        if(BpGetValue(bitmap, i)) continue;

        LockPage((void*) (i * PAGE_SIZE));
        return (void*) (i * PAGE_SIZE);
    }

    return NULL;
}

uint64_t GetFreeMemory() {
    return freeMemory;
}

uint64_t GetUsedMemory() {
    return usedMemory;
}

uint64_t GetReservedMemory() {
    return reservedMemory;
}
