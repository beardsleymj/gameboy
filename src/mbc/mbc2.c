#include "cart.h"

u8 mbc2_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}

	if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = cart.rom_bank & cart.rom_bank_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}

	if (address >= 0xA000 && address <= 0xA1FF)
	{
		if (cart.ram_bank_enable == true)
		{
			return cart.sram[address & 0x1FF];
		}
	}

	return 0xFF;
}

void mbc2_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	// Least significant bit of the upper byte must be 0 to enable / disable ram
		if ((value & 0x100) == 0)
		{
			cart.ram_bank_enable = (value & 0x0A) == 0x0A;
		}
		return;
	}
	
	if (address >= 0x2000 && address <= 0x3FFF)
	{	// bank 1 register
		if ((value & 0x100) == 0)
		{
			cart.rom_bank = (value & 0xF);
			if (cart.rom_bank == 0)
				cart.rom_bank = 1;
		}
		return;
	}
	
	if (address >= 0xA000 && address <= 0xA1FF && cart.ram_bank_enable == true)
	{	// RAM write
		if (cart.ram_bank_enable)
		{
			cart.sram[address & 0x1FFF] = value & 0x0F;
		}
		return;
	}
}