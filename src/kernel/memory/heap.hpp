#pragma once
#include <stdint.h>
#include <stddef.h>
#include "memory.hpp"

struct HeapSegmentHeader {
    size_t length;
    HeapSegmentHeader* next;
    HeapSegmentHeader* last;
    bool free;

    HeapSegmentHeader* Split(size_t splitLength);
    void CombineForward();
    void CombineBackward();
};

extern void* _heapStart;
extern void* _heapEnd;

class Heap {
public:
    void Initialize(void* heapAddress, size_t pageCount);
    void Expand(size_t length);

    static Heap GetInstance();
};

extern Heap _globalHeap;
