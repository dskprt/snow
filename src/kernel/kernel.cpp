#include "kernel.hpp"

extern uint64_t kstart;
extern uint64_t kend;

extern "C" void kmain(BootInfo* boot) {
    _globalFrameAllocator = PageFrameAllocator();
    PageFrameAllocator::GetInstance().Initialize(boot->memMap, boot->memMapSize, boot->memMapDescSize);

    uint64_t ksize = (uint64_t) &kend - (uint64_t) &kstart;
    PageFrameAllocator::GetInstance().LockPages(&kstart, (uint64_t) (ksize / Memory::PAGE_SIZE) + 1);

    PageTable* PML4 = (PageTable*) PageFrameAllocator::GetInstance().RequestPage();
    memset(PML4, 0, Memory::PAGE_SIZE);

    _globalTableManager = PageTableManager();
    PageTableManager::GetInstance().Initialize(PML4);

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
    PageTableManager::GetInstance().MapMemory((void*) 0x600000000, (void*) 0x80000);

    memset(boot->framebuffer->Address, 0, boot->framebuffer->BufferSize); // clear screen

    _globalHeap = Heap();
    Heap::GetInstance().Initialize((void*) 0x0000100000000000, 0x10);

    uint64_t* test = (uint64_t*) 0x600000000;
    *test = 26;

    char str[32];
    itoa(*test, str, 10);

    Graphics::DrawString(boot->framebuffer, boot->font, str, 5, 5, 0xFFFFFFFF);

    /*int cursorX = 5;
    int cursorY = 5;

    char buf[64];

    Graphics::DrawString(boot->framebuffer, boot->font, "Free memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += boot->font->GetTextWidth("Free memory: ");
    itoa(allocator.GetFreeMemory(), buf, 10);
    Graphics::DrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;

    Graphics::DrawString(boot->framebuffer, boot->font, "Used memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += boot->font->GetTextWidth("Used memory: ");
    itoa(allocator.GetUsedMemory(), buf, 10);
    Graphics::DrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;

    Graphics::DrawString(boot->framebuffer, boot->font, "Reserved memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += boot->font->GetTextWidth("Reserved memory: ");
    itoa(allocator.GetReservedMemory(), buf, 10);
    Graphics::DrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;*/

    // char totalMem[64];
    // itoa(GetTotalMemory(boot->memMap,
    //     boot->memMapSize / boot->memMapDescSize,
    //     boot->memMapDescSize), totalMem, 10);

    // GfxDrawString(boot->framebuffer, boot->font, totalMem, 400, 5, 0xFFFFFFFF);

    // uint8_t buffer[32] = { 0 };

    // Bitmap bp = BpCreate((uint8_t*) &buffer, 32);
    // BpSetValue(bp, 0, true);
    // BpSetValue(bp, 1, true);
    // BpSetValue(bp, 2, true);
    // BpSetValue(bp, 5, true);
    // BpSetValue(bp, 6, true);
    // BpSetValue(bp, 9, true);
    // BpSetValue(bp, 11, true);
    // BpSetValue(bp, 18, true);

    // int cursorY = 50;

    // for(int i = 0; i < 20; i++) {
    //     GfxDrawString(boot->framebuffer, boot->font, BpGetValue(bp, i) == 1 ? "true" : "false", 400, cursorY, 0xFFFFFFFF);
    //     cursorY += boot->font->header->charsize;
    // }

    // uint64_t memMapEntries = boot->memMapSize / boot->memMapDescSize;

    // int cursorX = 5;
    // cursorY = 5;

    // char* name = "EfiConventionalMemory: ";

    // for(int i = 0; i < memMapEntries; i++) {
    //     EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) boot->memMap + (i * boot->memMapDescSize));

    //     if(desc->type == 7) {
    //         GfxDrawString(boot->framebuffer, boot->font, name, cursorX, cursorY, 0xFFFFFFFF);
    //         cursorX += GetTextWidth(boot->font, name);

    //         char size[32];
    //         itoa(desc->numPages * 4096 / 1024, size, 10);

    //         GfxDrawString(boot->framebuffer, boot->font, size, cursorX, cursorY, 0x38cf9aff);
    //         cursorX += GetTextWidth(boot->font, size);

    //         GfxDrawString(boot->framebuffer, boot->font, " KB", cursorX, cursorY, 0x38cf9aff);

    //         cursorX = 0;
    //         cursorY += boot->font->header->charsize;
    //     }
    // }

    while(true);
}
