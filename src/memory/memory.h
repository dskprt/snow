#pragma once
#include <stdint.h>
#include "../efi/api.h"

extern const int PAGE_SIZE;

extern uint64_t totalMem;
uint64_t GetTotalMemory(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t entries, uint64_t descSize);
