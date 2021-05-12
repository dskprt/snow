#include "mem.hpp"

void memcpy(char* dest, char* src, int len) {
    for(int i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

void* memset(void* start, int value, size_t num) {
    unsigned char* p = (unsigned char*) start;
    unsigned char x = value & 0xff;

    while(num--) {
        *p++ = x;
    }

    return start;
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
