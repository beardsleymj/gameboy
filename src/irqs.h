#pragma once
#include "gb.h"
void handle_interrupts(gameboy* gb);
void dispatch_interrupt(gameboy* gb, uint16_t vec);