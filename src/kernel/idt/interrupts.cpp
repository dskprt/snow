#include "interrupts.hpp"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame) {
    Graphics::DrawString(bootInfo->framebuffer, bootInfo->font, "Page fault detected",
        bootInfo->framebuffer->Width - (19 * (bootInfo->font->header->charsize / 2)) / 2,
        bootInfo->framebuffer->Height - (bootInfo->font->header->charsize / 2),
        0xFFFFFFFF);

    while(true);
}
