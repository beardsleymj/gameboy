#include "cart.h"
#include "mbc.h"
#include "gb.h"

cartridge_t cart;

void cart_load(char* rom_path)
{
	FILE* rom = fopen(rom_path, "rb");
	if (rom == NULL) 
	{
		printf("Error opening ROM");
		exit(-1);
	}

	u8 code;
	u16 banks;

	// ROM 
	fseek(rom, 0x148, SEEK_SET);
	fread(&code, 1, 1, rom);
	switch (code)
	{
		case 0: banks = 2; cart.rom_address_pins_mask = 0x01; break;
		case 1: banks = 4; cart.rom_address_pins_mask = 0x03; break;
		case 2: banks = 8; cart.rom_address_pins_mask = 0x07; break;
		case 3: banks = 16; cart.rom_address_pins_mask = 0x0F; break;
		case 4: banks = 32; cart.rom_address_pins_mask = 0x1F; break;
		case 5: banks = 64;	cart.rom_address_pins_mask = 0x3F; break;
		case 6: banks = 128; cart.rom_address_pins_mask = 0x7F; break;
		case 7: banks = 256; cart.rom_address_pins_mask = 0xFF; break;
		case 8: banks = 512; cart.rom_address_pins_mask = 0x1FF; break;
		default: exit(-3);
	}
	cart.rom_banks = banks;

	cart.rom = malloc((size_t)banks * ROM_BANK_SIZE);
	rewind(rom);
	fread(cart.rom, (size_t)banks * ROM_BANK_SIZE, 1, rom);

	// SRAM
	cart.cart_type = cart.rom[0x149];
	switch (cart.cart_type)
	{
		case 0: banks = 0; break;
		case 2: banks = 1; break;
		case 3: banks = 4; break;
		case 4: banks = 16; break;
		case 5: banks = 8; break;
		default: exit(-3);
	}
	cart.ram_banks = banks;
	cart.sram = calloc(1, (size_t)banks * RAM_BANK_SIZE);

	memcpy(&cart.title, &cart.rom[0x0134], 16);
	cart.cgb_flag = cart.rom[0x143];
	cart.cart_type = cart.rom[0x147];
	printf("cart type: %x\n", cart.cart_type);

	cart.ram_bank_enable = 0;
	cart.bank1_reg = 1;
	cart.bank2_reg = 0;
	cart.banking_mode = 0;

	printf("%s\n\n", cart.title);
	fclose(rom);
}

u8 cart_read_byte(u16 address)
{
	switch (cart.cart_type)
	{
		case 0: 
			if (address >= 0 && address <= 0x7FFF)
				return cart.rom[address];
			break;

		case 1: case 2: case 3: /* MBC1 */
			return mbc1_read(address);
			break;

		case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13: // MBC3
			return mbc3_read(address); 
			break;

		default:
			printf("MBC Not Implemented.\n");
			break;
	}
}

void write_cart_byte(u16 address, u8 value)
{
	switch (cart.cart_type)
	{
		case 0x0: // MBC0
			return;
			break;

		case 0x1: case 0x2: case 0x3:
			mbc1_write(address, value);
			break;

		case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13: // MBC3
			mbc3_write(address, value);
			break;

		default:
			printf("MBC Not Implemented.\n");
			break;
	}
}