#pragma once
#include "../types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct
{
	u8 seconds;
	u8 minutes;
	u8 hours;
	u16 day;
	bool halt;
	bool day_carry_bit;
} rtc_t;
	
typedef struct 
{
	char title[16];
	bool cgb_flag;
	u8 cart_type;

	u32 rom_size;
	u8* rom;

	u8 rom_bank;
	u8 ram_bank;

	u32 ram_size;
	u8 ram_banks;
	u16 ram_bank_mask;
	u8* sram;
	u8* ram_bank_ptr;

	FILE* savefile;

	u16 rom_bank_mask;
	bool ram_bank_enable;
	u8 banking_mode;

	rtc_t rtc;
} cartridge_t;

extern cartridge_t cart;

void cart_load(char* rom_path);
u8 cart_read_byte(u16 address);
void cart_write_byte(u16 address, u8 value);
void cart_write_save();
void rtc_update();

#ifdef __cplusplus
}
#endif