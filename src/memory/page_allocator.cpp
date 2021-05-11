#include "page_allocator.h"

void PageFrameAllocator::Initialize(EFI_MEMORY_DESCRIPTOR* map, size_t mapSize, size_t descSize) {
    if(initialized) return;

    uint64_t entries = mapSize / descSize;

    void* largestFreeMemorySegment = NULL;
    size_t largestFreeMemorySegmentSize = 0;

    for(int i = 0; i < entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) map + (i * descSize));

        if(desc->type == 7) { // EfiConventionalMemory
            if(desc->numPages * Memory::PAGE_SIZE > largestFreeMemorySegmentSize) {
                largestFreeMemorySegment = desc->physAddr;
                largestFreeMemorySegmentSize = desc->numPages * Memory::PAGE_SIZE;
            }
        }
    }

    uint64_t totalMemory = Memory::GetTotalMemory(map, entries, descSize);
    freeMemory = totalMemory;

    uint64_t bitmapSize = ((totalMemory / Memory::PAGE_SIZE) / 8) + 1;

    InitializeBitmap(bitmapSize, largestFreeMemorySegment);
    LockPages(&bitmap, (bitmap.size / Memory::PAGE_SIZE) + 1);

    for(int i = 0; i < entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) map + (i * descSize));

        if(desc->type != 7) { // not EfiConventionalMemory
            ReservePages(desc->physAddr, desc->numPages);
        }
    }

    initialized = true;
}

void PageFrameAllocator::LockPage(void* address) {
    uint64_t index = (uint64_t) address / Memory::PAGE_SIZE;
    if(bitmap.GetValue(index)) return;

    bitmap.SetValue(index, false);
    freeMemory -= Memory::PAGE_SIZE;
    usedMemory += Memory::PAGE_SIZE;
}

void PageFrameAllocator::LockPages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        LockPage((void*) ((uint64_t) address + (i * Memory::PAGE_SIZE)));
    }
}

void PageFrameAllocator::FreePage(void* address) {
    uint64_t index = (uint64_t) address / Memory::PAGE_SIZE;
    if(!bitmap.GetValue(index)) return;

    bitmap.SetValue(index, false);
    freeMemory += Memory::PAGE_SIZE;
    usedMemory -= Memory::PAGE_SIZE;
}

void PageFrameAllocator::FreePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        FreePage((void*) ((uint64_t) address + (i * Memory::PAGE_SIZE)));
    }
}

void* PageFrameAllocator::RequestPage() {
    for(uint64_t i = 0; i < bitmap.size * 8; i++) {
        if(bitmap.GetValue(i)) continue;

        LockPage((void*) (i * Memory::PAGE_SIZE));
        return (void*) (i * Memory::PAGE_SIZE);
    }

    return NULL;
}

uint64_t PageFrameAllocator::GetFreeMemory() {
    return freeMemory;
}

uint64_t PageFrameAllocator::GetUsedMemory() {
    return usedMemory;
}

uint64_t PageFrameAllocator::GetReservedMemory() {
    return reservedMemory;
}

void PageFrameAllocator::InitializeBitmap(size_t size, void* bufferAddress) {
    bitmap.buffer = (uint8_t*) bufferAddress;
    bitmap.size = size;

    for(int i = 0; i < size; i++) {
        *(uint8_t*) (bitmap.buffer + i) = 0;
    }
}

void PageFrameAllocator::ReservePage(void* address) {
    uint64_t index = (uint64_t) address / Memory::PAGE_SIZE;
    if(bitmap.GetValue(index)) return;

    bitmap.SetValue(index, true);
    freeMemory -= Memory::PAGE_SIZE;
    reservedMemory += Memory::PAGE_SIZE;
}

void PageFrameAllocator::ReservePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        ReservePage((void*) ((uint64_t) address + (i * Memory::PAGE_SIZE)));
    }
}

void PageFrameAllocator::ReleasePage(void* address) {
    uint64_t index = (uint64_t) address / Memory::PAGE_SIZE;
    if(!bitmap.GetValue(index)) return;

    bitmap.SetValue(index, false);
    freeMemory += Memory::PAGE_SIZE;
    reservedMemory -= Memory::PAGE_SIZE;
}

void PageFrameAllocator::ReleasePages(void* address, uint64_t count) {
    for(int i = 0; i < count; i++) {
        ReleasePage((void*) ((uint64_t) address + (i * Memory::PAGE_SIZE)));
    }
}
