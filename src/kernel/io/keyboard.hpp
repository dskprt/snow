#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../terminal/terminal.hpp"

enum Key {
    Escape = 0x01,
    Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
    Hyphen, Equal, Backspace, Tab,
    Q, W, E, R, T, Y, U, I, O, P,
    BracketOpen, BracketClose, Enter, LCtrl,
    A, S, D, F, G, H, J, K, L,
    SemiColon, SingleQuote, BackTick, LShift, Backslash,
    Z, X, C, V, B, N, M,
    Comma, Period, Slash, RShift, NumPadMultiply, LAlt, Space, CapsLock,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
    NumLock, ScrollLock,
    NumPad7, NumPad8, NumPad9, NumPadMinus,
    Numpad4, NumPad5, NumPad6, NumPadAdd,
    NumPad1, NumPad2, NumPad3, NumPad0, NumPadPeriod,
    F11 = 0x57, F12
};

class Keyboard {
public:
    static bool shift;
    static bool capsLock;

    static void HandleKeypress(uint8_t scancode, bool release);
private:
    static void _Handle(char c);
};
