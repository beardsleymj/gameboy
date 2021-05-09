#include "cart.h"
#include "mbc1.h"

void cart_load(gameboy* gb, char* rom_path)
{
	FILE* rom = fopen(rom_path, "rb");
	if (rom == NULL) 
	{
		printf("Error opening ROM");
		exit(-1);
	}

	uint8_t code;
	uint16_t banks;

	// ROM 
	fseek(rom, 0x148, SEEK_SET);
	fread(&code, 1, 1, rom);
	switch (code)
	{
		case 0: banks = 2; gb->rom_address_pins_mask = 0x01; break;
		case 1: banks = 4; gb->rom_address_pins_mask = 0x03; break;
		case 2: banks = 8; gb->rom_address_pins_mask = 0x07; break;
		case 3: banks = 16; gb->rom_address_pins_mask = 0x0F; break;
		case 4: banks = 32; gb->rom_address_pins_mask = 0x1F; break;
		case 5: banks = 64;	gb->rom_address_pins_mask = 0x3F; break;
		case 6: banks = 128; gb->rom_address_pins_mask = 0x7F; break;
		case 7: banks = 256; gb->rom_address_pins_mask = 0xFF; break;
		case 8: banks = 512; gb->rom_address_pins_mask = 0x1FF; break;
		default: exit(-3);
	}
	gb->rom_banks = banks;

	gb->rom = malloc((size_t)banks * ROM_BANK_SIZE);
	rewind(rom);
	fread(gb->rom, (size_t)banks * ROM_BANK_SIZE, 1, rom);

	// SRAM
	gb->cart_type = gb->rom[0x149];
	switch (gb->cart_type)
	{
		case 0: banks = 0; break;
		case 2: banks = 1; break;
		case 3: banks = 4; break;
		case 4: banks = 16; break;
		case 5: banks = 8; break;
		default: exit(-3);
	}
	gb->ram_banks = banks;
	gb->sram = calloc(1, (size_t)banks * RAM_BANK_SIZE);

	memcpy(&gb->title, &gb->rom[0x0134], 16);
	gb->cgb_flag = gb->rom[0x143];
	gb->cart_type = gb->rom[0x147];
	printf("cart type: %x\n", gb->cart_type);

	gb->ram_bank_enable = 0;
	gb->bank1_reg = 1;
	gb->bank2_reg = 0;
	gb->banking_mode = 0;

	printf("%s\n\n", gb->title);
	fclose(rom);
}

uint8_t cart_read_byte(gameboy* gb, uint16_t address)
{
	switch (gb->cart_type)
	{
		case 0: 
			if (address >= 0 && address <= 0x7FFF)
				return gb->rom[address];
			break;
		case 1: case 2: case 3: case 0x13:
			return mbc1_read_byte(gb, address);
			break;
	}
}

void write_cart_byte(gameboy* gb, uint16_t address, uint8_t value)
{
	switch (gb->cart_type)
	{
		case 0x0: // MBC0
			printf("MBC0 write at %x\n", address);
			return;
			break;
		case 0x1: case 0x2: case 0x3: case 0x13: // MBC1
			mbc1_write_byte(gb, address, value);
		break;
	}
}