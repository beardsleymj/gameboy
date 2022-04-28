#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../types.h"
#include <stdio.h>

typedef enum banking_modes { simple, advanced } banking_modes;

typedef struct cartridge
{
	char title[16];
	bool cgb_flag;
	u8 cart_type;

	u32 rom_size;
	u8* rom;
	u8* rom_ptr; // ptr for the switchable bank

	u8 bank1_reg;
	u8 bank2_reg;

	u32 ram_size;
	u8 ram_banks;
	u8* ram_ptr; // for "faster reads
	u8* sram;
	u8* ram_bank_ptr;

	FILE* savefile;

	u16 rom_address_pins_mask;
	bool ram_bank_enable;
	banking_modes banking_mode;
} cartridge_t;

extern cartridge_t cart;

void cart_load(char* rom_path);
u8 cart_read_byte(u16 address);
void cart_write_byte(u16 address, u8 value);
void cart_write_save();

#ifdef __cplusplus
}
#endif