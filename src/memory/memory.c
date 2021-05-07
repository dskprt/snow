#include "memory.h"

const int PAGE_SIZE = 4096;

uint64_t GetTotalMemory(EFI_MEMORY_DESCRIPTOR* map, uint64_t entries, uint64_t descSize) {
    int totalMem;
    if(totalMem > 0) return totalMem;

    for(int i = 0; i < entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) map + (i * descSize));
        totalMem += desc->numPages * PAGE_SIZE;
    }

    return totalMem;
}
