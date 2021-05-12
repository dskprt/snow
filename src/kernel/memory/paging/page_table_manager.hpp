#pragma once
#include <stdint.h>
#include "paging.hpp"
#include "page_map_indexer.hpp"
#include "page_frame_allocator.hpp"
#include "../../../libc/mem.hpp"

class PageTableManager {
public:
    PageTable* PML4;

    void Initialize(PageTable* pml4address);
    void MapMemory(void* virtualMemory, void* physicalMemory);

    static PageTableManager GetInstance();
};

extern PageTableManager _globalTableManager;
