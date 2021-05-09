#include "gb.h"
#include "gb.h"

uint8_t mbc1_read_byte(gameboy* gb, uint16_t address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{	// BANK 0
		if (gb->banking_mode == simple)
			return gb->rom[address];
		else // advanced only used for multi cart, this should never be hit usually
		{
			uint16_t bank = (gb->bank2_reg << 5) & gb->rom_address_pins_mask;
			return gb->rom[((bank << 14) | address) ];
		}
	}	// BANK 01-nn
	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		uint16_t bank = ((gb->bank2_reg << 5) | (gb->bank1_reg)) & gb->rom_address_pins_mask;
		return gb->rom[(bank << 14) | (address & 0x3FFF)];
	}
	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (gb->ram_bank_enable == 0xA)
		{		
			if (gb->banking_mode == simple || gb->ram_banks == 1)
				return gb->sram[address & 0x1FFF];
			else if (gb->banking_mode == advanced)
			{
				if (gb->bank2_reg < gb->ram_banks)
					return gb->sram[(gb->bank2_reg << 13) | (address & 0x1FFF)];
				else
					return 0;
			}
		}
		else // ram banking disabled
			return 0xFF;
	}
}

void mbc1_write_byte(gameboy* gb, uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{	
		gb->ram_bank_enable = value & 0xF;
	}
	else if (address >= 0xA000 && address <= 0xBFFF && gb->ram_bank_enable == 0xA)
	{	// RAM write
		if (gb->banking_mode == simple || gb->ram_banks == 1)
			gb->sram[address & 0x1FFF] = value;
		else if (gb->banking_mode == advanced)
		{
			if (gb->bank2_reg < gb->ram_banks)
				gb->sram[(gb->bank2_reg << 13) | (address & 0x1FFF)] = value;
		}
	}
	else if (address >= 0x2000 && address <= 0x3FFF)
	{	// bank 1 register
		gb->bank1_reg = (value & 0x1F);
		if (gb->bank1_reg == 0)
			gb->bank1_reg = 1;
	}
	else if (address >= 0x4000 && address <= 0x5FFF)
	{	
		gb->bank2_reg = value & 0x03;
	}
	else if (address >= 0x6000 && address <= 0x7FFF)
	{	// banking mode select
		if (value == simple)
			gb->banking_mode = simple;
		if (value == advanced)
			gb->banking_mode = advanced;
	}
}