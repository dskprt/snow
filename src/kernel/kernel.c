#include "kernel.h"

void kmain(BootInfo* boot) {
    //GfxDrawString(boot->framebuffer, boot->font, "Hello world!", 50, 50, 0xFFFFFFFF);

    uint64_t memMapEntries = boot->memMapSize / boot->memMapDescriptorSize;

    int cursorX = 5;
    int cursorY = 5;

    char* name = "EfiConventionalMemory: ";

    for (int i = 0; i < memMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*) ((uint64_t) boot->memMap + (i * boot->memMapDescriptorSize));

        if(desc->type == 7) {
            GfxDrawString(boot->framebuffer, boot->font, name, cursorX, cursorY, 0xFFFFFFFF);
            cursorX += GetTextWidth(boot->font, name);

            char size[32];
            itoa(desc->numPages * 4096 / 1024, size, 10);

            GfxDrawString(boot->framebuffer, boot->font, size, cursorX, cursorY, 0x38cf9aff);
            cursorX += GetTextWidth(boot->font, size);

            GfxDrawString(boot->framebuffer, boot->font, " KB", cursorX, cursorY, 0x38cf9aff);

            cursorX = 0;
            cursorY += boot->font->header->charsize;
        }
    }

    //while(1);
}
