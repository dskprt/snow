#include "kernel.hpp"

extern uint64_t kstart;
extern uint64_t kend;

BootInfo* bootInfo;
IDTR idtr;

__attribute__((aligned(0x1000))) GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // null
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
    {0, 0, 0, 0x00, 0x00, 0}, // user null
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
};

extern "C" void kmain(BootInfo* boot) {
    bootInfo = boot;

    //{ gdt setup
    GDTDescriptor descriptor;
    descriptor.Size = sizeof(GDT) - 1;
    descriptor.Offset = (uint64_t) &DefaultGDT;

    LoadGDT(&descriptor);
    //}

    //{ memory setup
    _globalFrameAllocator = PageFrameAllocator();
    PageFrameAllocator::GetInstance().Initialize(boot->memMap, boot->memMapSize, boot->memMapDescSize);

    uint64_t ksize = (uint64_t) &kend - (uint64_t) &kstart;
    PageFrameAllocator::GetInstance().LockPages(&kstart, (uint64_t) (ksize / Memory::PAGE_SIZE) + 1);

    PageTable* PML4 = (PageTable*) PageFrameAllocator::GetInstance().RequestPage();
    memset(PML4, 0, Memory::PAGE_SIZE);

    _globalTableManager = PageTableManager();
    _globalTableManager.Initialize(PML4);

    for(uint64_t i = 0; i < Memory::GetTotalMemory(boot->memMap,
        boot->memMapSize / boot->memMapDescSize, boot->memMapDescSize); i += Memory::PAGE_SIZE) {

        PageTableManager::GetInstance().MapMemory((void*) i, (void*) i);
    }

    uint64_t fbBase = (uint64_t) boot->framebuffer->Address;
    uint64_t fbSize = (uint64_t) boot->framebuffer->BufferSize + Memory::PAGE_SIZE;

    PageFrameAllocator::GetInstance().LockPages((void*) fbBase, (fbSize / Memory::PAGE_SIZE) + 1);

    for(uint64_t i = fbBase; i < fbBase + fbSize; i += Memory::PAGE_SIZE) {
        PageTableManager::GetInstance().MapMemory((void*) i, (void*) i);
    }

    asm("mov %0, %%cr3" : : "r" (PML4));
    //}

    //{ interrupts setup
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t) PageFrameAllocator::GetInstance().RequestPage();

    IDTDescEntry* int_PageFault = (IDTDescEntry*) (idtr.Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t) PageFault_Handler);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;

    asm ("lidt %0" : : "m" (idtr));
    //}

    memset(boot->framebuffer->Address, 0, boot->framebuffer->BufferSize); // clear screen

    //{ heap initialization
    _globalHeap = Heap();
    Heap::GetInstance().Initialize((void*) 0x0000100000000000, 0x10);
    //}

    PageTableManager::GetInstance().MapMemory((void*) 0x600000000, (void*) 0x80000);

    uint64_t* test = (uint64_t*) 0x600000000;
    *test = 26;

    char str[32];
    itoa(*test, str, 10);

    Graphics::DrawString(boot->framebuffer, boot->font, str, 5, 5, 0xFFFFFFFF);
    
    while(true);
}
