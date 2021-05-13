#include "interrupts.hpp"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame) {
    Panic("Page fault detected");
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame) {
    Panic("Double fault detected");
}

__attribute__((interrupt)) void GeneralProtectionFault_Handler(struct interrupt_frame* frame) {
    Panic("General protection fault detected");
}
