#pragma once
#include "../kernel.hpp"
#include "../gfx/graphics.hpp"

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
