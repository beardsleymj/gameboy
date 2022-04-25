#pragma once
#include "gb.h"
#include <stdint.h>

#define ROM_BANK_SIZE 0x4000 // 16 KB
#define RAM_BANK_SIZE 0x2000 // 8 KB

typedef struct cartridge
{
	char title[16];
	u8 cgb_flag;
	u8 cart_type;

	// cartridge
	u16 rom_banks;
	u8* rom;
	u8* rom_ptr; // ptr for the switchable bank

	u8* sram_ptr;

	u8 bank1_reg;
	u8 bank2_reg;

	u8 ram_banks;
	u8* sram;
	u8* ram_bank_ptr;

	u16 rom_address_pins_mask;


	bool ram_bank_enable;
	banking_modes banking_mode;
} cartridge_t;

extern cartridge_t cart;

void cart_load(char* rom);
u8 cart_read_byte(u16 address);
void write_cart_byte(u16 address, u8 value);
