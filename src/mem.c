#include "mem.h"
#include "timing.h"
#include "cart.h"

// Todo: something faster
uint8_t read_byte(gameboy* gb, uint16_t address) 
{
	gb->cycles += 4;

	if (gb->bootrom_disabled == 0 && address < 0x100)
		return gb->bootrom[address];
	else if (address <= 0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
	{
		return cart_read_byte(gb, address);
	}
	else if (address == JOYP)
	{
		gb->joypad->unused = 2;
		if (!gb->joypad->action && gb->joypad->direction) 
		{
			gb->joypad->down_start = gb->start;
			gb->joypad->up_select = gb->select;
			gb->joypad->left_b = gb->b;
			gb->joypad->right_a = gb->a;
		}
		else if (gb->joypad->action && !gb->joypad->direction)
		{
			gb->joypad->down_start = gb->down;
			gb->joypad->up_select = gb->up;
			gb->joypad->left_b = gb->left;
			gb->joypad->right_a = gb->right;
		}
		return gb->mem[JOYP];
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF)
		return 0;
	else
		return gb->mem[address];
}

// Todo: something faster
void write_byte(gameboy* gb, uint16_t address, uint8_t value) 
{
	gb->cycles += 4;

	// Print serial data
	if (address == 0xFF02)
		printf("%c", gb->mem[0xFF01]);
	if (address == 0xFF50)
		gb->bootrom_disabled = 1;

	if (address == TIMA)
		reset_div(gb);
	else if (address == TAC)
		write_tac(gb, value);
	else if (address == JOYP)
		gb->mem[address] = value & 0xF0;
	else if (address <=  0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
		write_cart_byte(gb, address, value);
	else if (address >= 0xE000 && address <= 0xFDFF)
		printf("Illegal write to echo ram: %x\n", address);
	else if (address >= 0xFEA0 && address <= 0xFEFF)
		printf("write: illegal write to prohibited area: %x\n", address);
	else
		gb->mem[address] = value;

	if (address == DMA)
		dma_transfer(gb);
}

uint16_t read_word(gameboy* gb, uint16_t address)
{
	return (read_byte(gb, address + 1) << 8) |  read_byte(gb, address);
}


// TODO: Actually Implement dma tranfers cycles properly
void dma_transfer(gameboy* gb)
{
	uint16_t address = gb->mem[DMA] * 0x100;
	for (int i = 0; i < 0xA0; i++)
	{
		write_byte(gb, OAM_START + i, read_byte(gb, address + i));
		gb->cycles -= 1280;
	}
}

