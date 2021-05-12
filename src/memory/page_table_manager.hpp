#pragma once
#include <stdint.h>
#include "paging.hpp"
#include "page_map_indexer.hpp"
#include "page_frame_allocator.hpp"
#include "../libc/mem.hpp"

class PageTableManager {
public:
    PageTableManager(PageTable* pml4address);
    void MapMemory(void* virtualMemory, void* physicalMemory);

    PageTable* PML4;
};
