#pragma once
#include <stdint.h>
#include "paging.h"
#include "page_map_indexer.h"
#include "page_frame_allocator.h"
#include "../libc/mem.h"

class PageTableManager {
public:
    PageTableManager(PageTable* pml4address);
    void MapMemory(void* virtualMemory, void* physicalMemory);

    PageTable* PML4;
};
