#include "heap.hpp"
#include "paging/page_table_manager.hpp"
#include "paging/page_frame_allocator.hpp"

void* _heapStart;
void* _heapEnd;

HeapSegmentHeader* lastHeader;
Heap _globalHeap;

Heap Heap::GetInstance() {
    return _globalHeap;
}

void Heap::Initialize(void* heapAddress, size_t pageCount) {
    void* pos = heapAddress;

    for(size_t i = 0; i < pageCount; i++) {
        PageTableManager::GetInstance().MapMemory(pos, PageFrameAllocator::GetInstance().RequestPage());
        pos = (void*) ((size_t) pos + Memory::PAGE_SIZE);
    }

    size_t heapLength = pageCount * Memory::PAGE_SIZE;

    _heapStart = heapAddress;
    _heapEnd = (void*) ((size_t) _heapStart + heapLength);

    HeapSegmentHeader* startSegment = (HeapSegmentHeader*) heapAddress;

    startSegment->length = heapLength - sizeof(HeapSegmentHeader);
    startSegment->next = NULL;
    startSegment->last = NULL;
    startSegment->free = true;

    lastHeader = startSegment;
}

void Heap::Expand(size_t length) {
    if(length % Memory::PAGE_SIZE) {
        length -= length % Memory::PAGE_SIZE;
        length += Memory::PAGE_SIZE;
    }

    size_t pageCount = length / Memory::PAGE_SIZE;
    HeapSegmentHeader* newSegment = (HeapSegmentHeader*) _heapEnd;

    for(size_t i = 0; i < pageCount; i++){
        PageTableManager::GetInstance().MapMemory(_heapEnd, PageFrameAllocator::GetInstance().RequestPage());
        _heapEnd = (void*) ((size_t) _heapEnd + Memory::PAGE_SIZE);
    }

    newSegment->free = true;
    newSegment->last = lastHeader;

    lastHeader->next = newSegment;
    lastHeader = newSegment;

    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegmentHeader);
    newSegment->CombineBackward();
}

HeapSegmentHeader* HeapSegmentHeader::Split(size_t splitLength){
    if (splitLength < Memory::PAGE_SIZE) return NULL;
    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegmentHeader));
    if (splitSegLength < Memory::PAGE_SIZE) return NULL;

    HeapSegmentHeader* splitHeader = (HeapSegmentHeader*) ((size_t) this + splitLength + sizeof(HeapSegmentHeader));
    next->last = splitHeader;
    splitHeader->next = next;
    next = splitHeader;
    splitHeader->last = this;
    splitHeader->length = splitSegLength;
    splitHeader->free = free;
    length = splitLength;

    if(lastHeader == this) lastHeader = splitHeader;
    return splitHeader;
}

void HeapSegmentHeader::CombineForward() {
    if(!next) return;
    if(!next->free) return;

    if(next == lastHeader) lastHeader = this;
    if(next->next) next->next->last = this;

    length = length + next->length + sizeof(HeapSegmentHeader);
}

void HeapSegmentHeader::CombineBackward() {
    if(last && last->free) last->CombineForward();
}
