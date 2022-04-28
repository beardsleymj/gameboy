#include "mbc.h"
#include "cart.h"

u8 mbc5_read(u16 address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	
		return cart.rom[address];
	}	
	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		u16 bank = ((cart.bank2_reg << 8) | (cart.bank1_reg)) & cart.rom_address_pins_mask;
		return cart.rom[(bank << 14) | (address & 0x3FFF)];
	}
	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		return cart.sram[(cart.banking_mode << 13) | (address & 0x1FFF)];

		//if (cart.ram_bank_enable == true)
		//{		
		//	if (cart.banking_mode == simple || cart.ram_banks == 1)
		//		return cart.sram[address & 0x1FFF];
		//	else if (cart.banking_mode == advanced)
		//	{
		//		if (cart.bank2_reg < cart.ram_banks)
		//			return cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)];
		//	}
		//}
	}

	return 0xFF;
}

void mbc5_write(u16 address, u8 value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	// RAM Enable
		cart.ram_bank_enable = (value & 0x0A) == 0x0A;
	}
	else if (address >= 0x2000 && address <= 0x2FFF)
	{	// Low bit of ROM Bank Number
		cart.bank1_reg = value;
	}
	else if (address >= 0x3000 && address <= 0x3FFF)
	{	// High bit of ROM Bank Number
		cart.bank2_reg = value & 0x01;
	}
	else if (address >= 0x4000 && address <= 0x5FFF)
	{	// RAM Bank Number
		cart.banking_mode = (value & 1);
	}
	else if (address >= 0xA000 && address <= 0xBFFF && cart.ram_bank_enable == true)
	{	// RAM write
		cart.sram[(cart.banking_mode << 13) | (address & 0x1FFF)] = value;
		//if (cart.banking_mode == simple || cart.ram_banks == 1)
		//	cart.sram[address & 0x1FFF] = value;
		//else if (cart.banking_mode == advanced)
		//{
		//	if (cart.bank2_reg < cart.ram_banks)
		//		cart.sram[(cart.bank2_reg << 13) | (address & 0x1FFF)] = value;
		//}
	}
}