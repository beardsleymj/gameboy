#include "mbc.h"

u8 mbc3_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}	
	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = (cart.bank1_reg) & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (cart.ram_bank_enable == false)
		{
			return 0xFF;
		}

		if (cart.banking_mode == simple)
			return cart.sram[((cart.bank2_reg * 0x2000) + address & 0x1FFF)];
	}
}

void mbc3_write(u16 address, u8 value)
{
	//printf("MBC3 Write: Address %X Value %X\n", address, value);
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		cart.ram_bank_enable = (value & 0x0A) == 0x0A;
	}
	else if (address >= 0x2000 && address <= 0x3FFF)
	{	
		cart.bank1_reg = value & 0x7F;
	}
	else if (address >= 0x4000 && address <= 0x5FFF)
	{
		cart.bank2_reg = value & 0x03;
	}
	else if (address >= 0x6000 && address <= 0x7FFF)
	{	
		cart.banking_mode = value & 1;
	}
	else if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable != 0)
	{	
		if (cart.banking_mode == simple)
			cart.sram[address & 0x1FFF] = value;
		else if (cart.banking_mode == advanced)
		{
			if (cart.bank2_reg < cart.ram_banks)
				cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)] = value;
			
		}
	}
}