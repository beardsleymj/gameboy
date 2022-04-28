#include "cart.h"
#include "mbc.h"
#include <stdlib.h>
#include <string.h>

#define ROM_BANK_SIZE 0x4000 // 16 KB
#define RAM_BANK_SIZE 0x2000 // 8 KB

cartridge_t cart;

void cart_load(char* rom_path)
{
	FILE* rom_file = fopen(rom_path, "rb");
	if (rom_file == NULL)
	{
		printf("Error opening ROM");
		exit(-1);
	}

	// ROM 
	u8 code;
	fseek(rom_file, 0x148, SEEK_SET);
	fread(&code, 1, 1, rom_file);
	switch (code)
	{
		case 0: cart.rom_size = 2   * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x01; break;
		case 1: cart.rom_size = 4   * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x03; break;
		case 2: cart.rom_size = 8   * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x07; break;
		case 3: cart.rom_size = 16  * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x0F; break;
		case 4: cart.rom_size = 32  * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x1F; break;
		case 5: cart.rom_size = 64  * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x3F; break;
		case 6: cart.rom_size = 128 * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x7F; break;
		case 7: cart.rom_size = 256 * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0xFF; break;
		case 8: cart.rom_size = 512 * ROM_BANK_SIZE; cart.rom_address_pins_mask = 0x1FF; break;
		default: exit(-3);
	}

	cart.rom = malloc((size_t)cart.rom_size);
	rewind(rom_file);
	fread(cart.rom, (size_t)cart.rom_size, 1, rom_file);

	switch (cart.rom[0x147])
	{
		case 0x00:
			cart.cart_type = 0;
			break;

		case 0x01: case 0x02: case 0x03:
			cart.cart_type = 1;
			break;

		case 0x05: case 0x06:
			cart.cart_type = 2;
			break;

		case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
			cart.cart_type = 3;
			break;

		case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E:
			cart.cart_type = 5;
			break;

		default:
			printf("Unknown cartridge type\n");
			exit(0);
			break;
	}

	// SRAM
	switch (cart.rom[0x149])
	{
		case 0: cart.ram_banks = 0; break;
		case 2: cart.ram_banks = 1; break;
		case 3: cart.ram_banks = 4; break;
		case 4: cart.ram_banks = 16; break;
		case 5: cart.ram_banks = 8; break;
		default: exit(-3);
	}
	cart.ram_size = cart.ram_banks * RAM_BANK_SIZE;
	cart.sram = calloc(1, cart.ram_size);

	// load save
	if (cart.ram_banks != 0)
	{
		char savepath[256];
		strcpy(savepath, rom_path);
		char* x;
		x = strchr(savepath, '.');
		strcpy(x, ".sav");
		cart.savefile = fopen(savepath, "rb+");
		if (cart.savefile == 0)
		{
			cart.savefile = fopen(savepath, "wb+");
		}
		else
		{
			fread(cart.sram, (size_t)cart.ram_size, 1, cart.savefile);
		}
	}

	memcpy(&cart.title, &cart.rom[0x0134], 16);
	cart.cgb_flag = (cart.rom[0x143] == 0x80) || (cart.rom[0x143] == 0xC0);

	cart.ram_bank_enable = 0;
	cart.bank1_reg = 1;
	cart.bank2_reg = 0;
	cart.banking_mode = 0;

	fclose(rom_file);
}

u8 cart_read_byte(u16 address)
{
	switch (cart.cart_type)
	{
		case 0: 
			if (address >= 0 && address <= 0x7FFF)
			return cart.rom[address];
			break;

		case 1:
			return mbc1_read(address);
			break;

		case 2:
			return mbc2_read(address);
			break;

		case 3:
			return mbc3_read(address); 
			break;

		case 5:
			return mbc5_read(address);
			break;

		default:
			printf("MBC Not Implemented. %i\n", cart.cart_type);
			exit(-1);
			break;
	}
}

void cart_write_byte(u16 address, u8 value)
{
	switch (cart.cart_type)
	{
		case 0:
			return;
			break;

		case 1:
			mbc1_write(address, value);
			break;

		case 2:
			mbc2_write(address, value);
			break;

		case 3:
			mbc3_write(address, value);
			break;

		case 5:
			mbc5_write(address, value);
			break;

		default:
			printf("MBC Not Implemented.\n");
			break;
	}
}

void cart_write_save()
{
	if (cart.savefile == NULL)
		return;

	fseek(cart.savefile, 0, SEEK_SET);
	fwrite(cart.sram, cart.ram_size, 1, cart.savefile);
	fclose(cart.savefile);
}