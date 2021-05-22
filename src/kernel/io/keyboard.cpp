#include "keyboard.hpp"

const char lookupTable[] = {
    0, 0,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    ',', '.', '/', 0, '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    '7', '8', '9', '-',
    '4', '5', '6', '+',
    '1', '2', '3', '0', '.',
    0, 0
};

bool Keyboard::shift = false;
bool Keyboard::capsLock = false;

void Keyboard::HandleKeypress(uint8_t scancode, bool release) {
    switch(scancode) {
        case CapsLock:
            if(release) break;

            if(capsLock) capsLock = false;
            else capsLock = true;
            break;
        case LShift:
        case RShift:
            if(release) shift = false;
            else shift = true;
            break;
        default:
            if(scancode < (sizeof(lookupTable) / sizeof(*lookupTable))) {
                char c = lookupTable[scancode];

                if(c != 0) {
                    _Handle(c);
                }
            } else if(scancode >= 0x81 && scancode <= 0xD8) {
                // release
                HandleKeypress((scancode ^ 0x80), true);
            }

            break;
    }
}

void Keyboard::_Handle(char c) {
    if(c == '\b') {
        Terminal::Backspace();
    } else if(c != '\t') {
        Terminal::PrintChar(c);
    }
}
