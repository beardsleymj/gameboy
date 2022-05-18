#include <time.h>
#include "cart.h"

u8 mbc3_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}

	if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = (cart.bank1_reg) & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}

	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (cart.ram_bank_enable == true)
		{
			if (cart.bank2_reg <= 3)
				return cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)];
			if (cart.bank2_reg >= 0x08 && cart.bank2_reg <= 0x0C)
			{
				rtc_update();
				if (cart.bank2_reg == 0x08) return cart.rtc.seconds;
				if (cart.bank2_reg == 0x09) return cart.rtc.minutes;
				if (cart.bank2_reg == 0x0A) return cart.rtc.hours;
				if (cart.bank2_reg == 0x0B) return cart.rtc.day & 0xFF;
				if (cart.bank2_reg == 0x0C) return (cart.rtc.day >> 8)
					| (cart.rtc.halt << 6)
					| (cart.rtc.day_carry_bit << 7)
					| 0b00111110;
			}
		}
	}
	
	return 0xFF;
}

void mbc3_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	// RAM an Timer Enable
		cart.ram_bank_enable = (value & 0x0A) == 0x0A;
		return;
	}
	
	if (address >= 0x2000 && address <= 0x3FFF)
	{	// ROM Bank Number
		cart.bank1_reg = value & 0x7F;
		cart.bank1_reg = cart.bank1_reg == 0 ? 1 : cart.bank1_reg;
		return;
	}

	if (address >= 0x4000 && address <= 0x5FFF)
	{	// RAM Bank Number - or - RTC Register Select
		cart.bank2_reg = value;
		return;
	}
	
	if (address >= 0x6000 && address <= 0x7FFF)
	{	// Latch Clock Data
		cart.banking_mode = (value & 1);
		cart.rtc.halt = (value >> 6) & 1;
		return;
	}

	if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable != 0)
	{	
		if (cart.bank2_reg <= 3)
			cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)] = value;

		if (cart.bank2_reg == 0x08) cart.rtc.seconds = value;
		if (cart.bank2_reg == 0x09) cart.rtc.minutes = value;
		if (cart.bank2_reg == 0x0A) cart.rtc.hours = value;
		if (cart.bank2_reg == 0x0B) cart.rtc.day = (cart.rtc.day & 0xFF00) | value;
		if (cart.bank2_reg == 0x0C)
		{
			cart.rtc.day = (cart.rtc.day & 0x00FF) | ((value & 0x01) << 8);
			cart.rtc.halt = (value >> 6) & 1;
			cart.rtc.day_carry_bit = (value >> 7) & 1;
		}						
	}
}

// current lazy idea for using host time eventually
void rtc_update()
{
	time_t rawtime;
	struct tm* timeinfo;
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	cart.rtc.seconds = timeinfo->tm_sec;
	cart.rtc.minutes = timeinfo->tm_min;
	cart.rtc.hours   = timeinfo->tm_hour;
	cart.rtc.day	 = timeinfo->tm_wday;
}