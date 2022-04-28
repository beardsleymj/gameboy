#include "mbc.h"
#include "cart.h"

u8 mbc2_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}	
	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = cart.bank1_reg & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	else if (address >= 0xA000 && address <= 0xA1FF)
	{
		if (cart.ram_bank_enable == true)
		{
			return cart.sram[address & 0x1FF];
		}
		else // ram banking disabled
			return 0xFF;
	}
	else
	{
		return 0xFF;
	}
}

void mbc2_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		// Least significant bit of the upper byte must be 0 to enable / disable ram
		if ((value & 0x100) == 0)
		{
			cart.ram_bank_enable = (value & 0x0A) == 0x0A;
		}
	}
	else if (address >= 0x2000 && address <= 0x3FFF)
	{	// bank 1 register
		if ((value & 0x100) == 0)
		{
			cart.bank1_reg = (value & 0xF);
			if (cart.bank1_reg == 0)
				cart.bank1_reg = 1;
		}
	}
	else if (address >= 0xA000 && address <= 0xA1FF && cart.ram_bank_enable == true)
	{	// RAM write
		if (cart.ram_bank_enable)
		{
			cart.sram[address & 0x1FFF] = value & 0x0F;
		}
	}
}