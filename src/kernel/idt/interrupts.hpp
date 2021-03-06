#pragma once
#include "../panic.hpp"
#include "../io/io.hpp"
#include "../io/ports.hpp"
#include "../io/keyboard.hpp"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame;

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void GeneralProtectionFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void FloatingPointException_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Overflow_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void BoundRangeExceeded_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void SIMD_FloatingPointException_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void InvalidOpcode_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Keyboard_Handler(struct interrupt_frame* frame);

void PIC_Remap();
void PIC_EndMaster();
void PIC_EndSlave();
