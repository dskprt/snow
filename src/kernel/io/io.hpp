#pragma once

inline void io_wait() {
    asm volatile("outb %%al, $0x80" : : "a" (0));
}
