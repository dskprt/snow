#pragma once
#include <stdint.h>
#include <stddef.h>

struct HeapSegmentHeader {
    size_t length;
    HeapSegmentHeader* next;
    HeapSegmentHeader* last;
    bool free;

    HeapSegmentHeader* Split(size_t splitLength);
    void CombineForward();
    void CombineBackward();
};

class Heap {
public:
    void* heapStart;
    void* heapEnd;

    void Initialize(void* heapAddress, size_t pageCount);
    void Expand(size_t length);

    static Heap GetInstance();
};

extern Heap _globalHeap;
