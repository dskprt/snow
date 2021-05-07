#include "kernel.h"

void kmain(BootInfo* boot) {
    PgInitialize(boot->memMap, boot->memMapSize, boot->memMapDescSize);

    int cursorX = 5;
    int cursorY = 5;

    char buf[64];

    GfxDrawString(boot->framebuffer, boot->font, "Free memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += GetTextWidth(boot->font, "Free memory: ");
    itoa(GetFreeMemory(), buf, 10);
    GfxDrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;

    GfxDrawString(boot->framebuffer, boot->font, "Used memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += GetTextWidth(boot->font, "Used memory: ");
    itoa(GetUsedMemory(), buf, 10);
    GfxDrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;

    GfxDrawString(boot->framebuffer, boot->font, "Reserved memory: ", cursorX, cursorY, 0xFFFFFFFF);
    cursorX += GetTextWidth(boot->font, "Reserved memory: ");
    itoa(GetReservedMemory(), buf, 10);
    GfxDrawString(boot->framebuffer, boot->font, buf, cursorX, cursorY, 0xFFFFFFFF);
    cursorY += boot->font->header->charsize;
    cursorX = 5;

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

    //while(1);
}
