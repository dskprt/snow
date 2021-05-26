#include "mem.hpp"

void* malloc(size_t size) {
    if(size % Memory::PAGE_SIZE > 0) {
        size -= (size % Memory::PAGE_SIZE);
        size += Memory::PAGE_SIZE;
    }

    if(size == 0) return NULL;

    HeapSegmentHeader* currentSegment = (HeapSegmentHeader*) _heapStart;

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

void* realloc(void* ptr, size_t oldSize, size_t newSize) { 
    void* newptr = malloc(oldSize);
    
    if(newptr == NULL) {
        return NULL;
    }

    memcpy((char*) newptr, (char*) ptr, newSize);
    free(ptr);

    return newptr;
}

void free(void* address) {
    HeapSegmentHeader* segment = (HeapSegmentHeader*) address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

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
