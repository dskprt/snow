#include "panic.hpp"

void Panic(char* message) {
    Graphics::DrawString(bootInfo->framebuffer, bootInfo->font, message,
        (bootInfo->framebuffer->Width / 2) - (19 * (bootInfo->font->header->charsize / 2)) / 2,
        (bootInfo->framebuffer->Height / 2) - (bootInfo->font->header->charsize / 2),
        0xFFFFFFFF);

    while(true);
}
