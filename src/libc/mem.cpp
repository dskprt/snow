#include "mem.hpp"
 
void *operator new(size_t size) {
    return malloc(size);
}
 
void *operator new[](size_t size) {
    return malloc(size);
}
 
void operator delete(void* p) {
    free(p);
}
 
void operator delete[](void* p) {
    free(p);
}

void* malloc(size_t size) {
    if(size % 0x10 > 0) {
        size -= (size % 0x10);
        size += 0x10;
    }

    if(size == 0) return NULL;

    HeapSegmentHeader* currentSegment = (HeapSegmentHeader*) Heap::GetInstance().heapStart;

    while(true) {
        if(currentSegment->free) {
            if(currentSegment->length > size) {
                currentSegment->Split(size);
                currentSegment->free = false;

                return (void*) ((uint64_t) currentSegment + sizeof(HeapSegmentHeader));
            }
            if(currentSegment->length == size) {
                currentSegment->free = false;

                return (void*) ((uint64_t) currentSegment + sizeof(HeapSegmentHeader));
            }
        }

        if(currentSegment->next == NULL) break;
        currentSegment = currentSegment->next;
    }

    Heap::GetInstance().Expand(size);
    return malloc(size);
}

void free(void* address) {
    HeapSegmentHeader* segment = (HeapSegmentHeader*) address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}
