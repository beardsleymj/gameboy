#include "cart.h"

u8 mbc5_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}	
	
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = ((cart.banking_mode << 8) | cart.rom_bank) & cart.rom_bank_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	
	if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable)
	{
		return cart.sram[(cart.ram_bank << 13) | (address & 0x1FFF)];
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
		cart.rom_bank = value;
		return;
	}
	
	if (address >= 0x3000 && address <= 0x3FFF)
	{	// High bit of ROM Bank Number
		cart.banking_mode = value & 0x01;
		return;
	}
	
	if (address >= 0x4000 && address <= 0x5FFF)
	{	// RAM Bank Number - games with rumble write to bit 3, not currently supported
		cart.ram_bank = value & cart.ram_bank_mask;
		return;
	}
	
	if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable)
	{	// RAM write
		cart.sram[(cart.ram_bank << 13) | (address & 0x1FFF)] = value;
		return;
	}
}