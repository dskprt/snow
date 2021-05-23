#include "console.hpp"

Framebuffer* Console::framebuffer;
PSF1_FONT* Console::font;

int Console::width = 0;
int Console::height = 0;
int Console::bufferWidth = 0;
int Console::bufferHeight = 0;

int Console::cursorX = 0;
int Console::cursorY = 0;

char* Console::buffer;
int* Console::colorBuffer;

void Console::Initialize(Framebuffer* _framebuffer, PSF1_FONT* _font) {
    framebuffer = _framebuffer;
    font = _font;

    width = floor(_framebuffer->Width / (_font->header->charsize / 2));
    height = floor(_framebuffer->Height / _font->header->charsize);

    bufferWidth = width;
    bufferHeight = height;
    
    buffer = new char[bufferWidth * bufferHeight];
    colorBuffer = new int[bufferWidth * bufferHeight];

    for(int i = 0; i < (bufferWidth * bufferHeight); i++) {
        buffer[i] = 0;
        colorBuffer[i] = 0;
    }
}

void Console::Write(char* str, int color) {
    char* text = str;

    while(*text != 0) {
        int index = cursorY * bufferWidth + cursorX;

        buffer[index] = *text;
        colorBuffer[index] = color;

        text++;
    }
}

void Console::Backspace() {
    cursorX--;
    
    if(cursorX < 0) {
        cursorY--;
        cursorX = bufferWidth - 1;
    }

    buffer[cursorY * bufferWidth + cursorX] = '\0';
}

void Console::Flush() {
    memset(bootInfo->framebuffer->Address, 0, bootInfo->framebuffer->BufferSize); // clear screen

    int x = 0;
    int y = 0;

    for(int i = 0; i < (bufferWidth * bufferHeight); i++) {
        char c = buffer[i];

        if(c == 0) break;
        if(c == '\n') {
            x = 0;
            y += font->header->charsize;
            continue;
        }

        Graphics::DrawChar(framebuffer, font, c, x, y, colorBuffer[i]);
        x += font->header->charsize / 2;

        if(x + (font->header->charsize / 2) > framebuffer->Width) {
            x = 0;
            y += font->header->charsize;
        }
    }
}
