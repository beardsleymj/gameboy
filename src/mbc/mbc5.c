#include "cart.h"

u8 mbc5_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}	
	
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = ((cart.bank2_reg << 8) | (cart.bank1_reg)) & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		return cart.sram[(cart.banking_mode << 13) | (address & 0x1FFF)];
	}

	return 0xFF;
}

void mbc5_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	// RAM Enable
		cart.ram_bank_enable = (value & 0x0A) == 0x0A;
		return;
	}

	if (address >= 0x2000 && address <= 0x2FFF)
	{	// Low byte of ROM Bank Number
		cart.bank1_reg = value;
		return;
	}
	
	if (address >= 0x3000 && address <= 0x3FFF)
	{	// High bit of ROM Bank Number
		cart.bank2_reg = value & 0x01;
		return;
	}
	
	if (address >= 0x4000 && address <= 0x5FFF)
	{	// RAM Bank Number
		cart.banking_mode = (value & 1);
		return;
	}
	
	if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable == true)
	{	// RAM write
		cart.sram[(cart.banking_mode << 13) | (address & 0x1FFF)] = value;
		return;
	}
}