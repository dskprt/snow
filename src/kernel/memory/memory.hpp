#pragma once
#include <stdint.h>
#include "../../efi/api.h"

class Memory {
public:
    static const int PAGE_SIZE = 4096;
    static uint64_t GetTotalMemory(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t entries, uint64_t descSize);
};
