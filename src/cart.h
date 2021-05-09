#pragma once
#include <stdint.h>
#include "gb.h"

#define ROM_BANK_SIZE 0x4000 // 16 KB
#define RAM_BANK_SIZE 0x2000 // 8 KB

void cart_load(gameboy* gb, char* rom);
uint8_t cart_read_byte(gameboy* gb, uint16_t address);
void write_cart_byte(gameboy* gb, uint16_t address, uint8_t value);
