#include "mbc.h"

// should be working(?)

u8 mbc1_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	// BANK 0
		return cart.rom[address];
	}	// BANK 01-nn
	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = ((cart.bank2_reg << 5) | (cart.bank1_reg)) & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (cart.ram_bank_enable == true)
		{		
			if (cart.banking_mode == simple || cart.ram_banks == 1)
				return cart.sram[address & 0x1FFF];
			else if (cart.banking_mode == advanced)
			{
				if (cart.bank2_reg < cart.ram_banks)
					return cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)];
				else
					return 0;
			}
		}
		else // ram banking disabled
			return 0xFF;
	}
}

void mbc1_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	
		cart.ram_bank_enable = (value & 0x0A) == 0x0A;
	}
	else if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable == true)
	{	// RAM write
		if (cart.banking_mode == simple || cart.ram_banks == 1)
			cart.sram[address & 0x1FFF] = value;
		else if (cart.banking_mode == advanced)
		{
			if (cart.bank2_reg < cart.ram_banks)
				cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)] = value;
		}
	}
	else if (address >= 0x2000 && address <= 0x3FFF)
	{	// bank 1 register
		cart.bank1_reg = (value & 0x1F);
		if (cart.bank1_reg == 0)
			cart.bank1_reg = 1;
	}
	else if (address >= 0x4000 && address <= 0x5FFF)
	{	
		cart.bank2_reg = value & 0x03;
	}
	else if (address >= 0x6000 && address <= 0x7FFF)
	{	// banking mode select
		cart.banking_mode = (value & 1);
	}
}