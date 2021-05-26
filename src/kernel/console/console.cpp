#include "console.hpp"

Framebuffer* Console::framebuffer;
PSF1_FONT* Console::font;

int _console_width;
int _console_height;
int _console_bufferWidth;
int _console_bufferHeight;

int _console_cursorX;
int _console_cursorY;

char* Console::buffer;
int* Console::colorBuffer;

void Console::Initialize(Framebuffer* _framebuffer, PSF1_FONT* _font) {
    framebuffer = _framebuffer;
    font = _font;

    _console_width = floor(_framebuffer->Width / (_font->header->charsize / 2));
    _console_height = floor(_framebuffer->Height / _font->header->charsize);

    _console_bufferWidth = _console_width;
    _console_bufferHeight = _console_height;
    
    buffer = (char*) malloc(_console_bufferWidth * _console_bufferHeight);
    colorBuffer = (int*) malloc(_console_bufferWidth * _console_bufferHeight);

    for(int i = 0; i < (_console_bufferWidth * _console_bufferHeight); i++) {
        buffer[i] = 0;
        colorBuffer[i] = 0;
    }
}

void Console::Write(char* str, int color) {
    char* text = str;

    while(*text != 0) {
        if(*text == '\b') {
            Backspace();

            text++;
            continue;
        }

        int index = _console_cursorY * _console_bufferWidth + _console_cursorX;

        buffer[index] = *text;
        colorBuffer[index] = color;

        text++;
    }
}

void Console::Backspace() {
    _console_cursorX--;
    
    if(_console_cursorX < 0) { // TODO fix
        _console_cursorY--;

        uint8_t done = 0;
        
        for(uint16_t i = _console_bufferWidth - 1; i >= 0; i--) {
            int index = _console_cursorY * _console_bufferWidth + i;

            if(buffer[index] != '\0') {
                _console_cursorX = i + 1;
                done = 1;
                break;
            }
        }

        if(!done) {
            _console_cursorX = 0;
        }
    }

    buffer[_console_cursorY * _console_bufferWidth + _console_cursorX] = 127;
}

void Console::Flush() {
    //memset(bootInfo->framebuffer->Address, 0, bootInfo->framebuffer->BufferSize); // clear screen

    int x = 0;
    int y = 0;

    for(int i = 0; i < (_console_bufferWidth * _console_bufferHeight); i++) {
        char c = buffer[i];

        if(c == 0) {
            continue;
        } else if(c == 127) {
            for(unsigned long _y = y; _y < y + 16; _y++) {
                for(unsigned long _x = x; _x < x + 8; _x++) {
                    *(unsigned int*)(framebuffer->Address + _x + (_y * framebuffer->PixelsPerScanLine)) = 0;
                }
            }

            continue;
        } else if(c == '\n') {
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
