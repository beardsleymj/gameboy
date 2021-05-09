#include "irqs.h"

void handle_interrupts(gameboy* gb)
{
	//check Interrupts
	if (gb->IME && gb->interrupt_enable->vblank && gb->interrupt_flag->vblank)
	{
		dispatch_interrupt(gb, 0x40);
		gb->interrupt_flag->vblank = 0;
	}
	else if (gb->IME && gb->interrupt_enable->stat && gb->interrupt_flag->stat)
	{
		dispatch_interrupt(gb, 0x48);
		gb->interrupt_flag->stat = 0;
	}
	else if (gb->IME && gb->interrupt_enable->timer && gb->interrupt_flag->timer)
	{
		dispatch_interrupt(gb, 0x50);
		gb->interrupt_flag->timer = 0;
	}
	else if (gb->IME && gb->interrupt_enable->serial && gb->interrupt_flag->serial)
	{
		dispatch_interrupt(gb, 0x58);
		gb->interrupt_flag->serial = 0;
	}
	else if (gb->IME && gb->interrupt_enable->joypad && gb->interrupt_flag->joypad)
	{
		dispatch_interrupt(gb, 0x60);
		gb->interrupt_flag->joypad = 0;
	}
}

void dispatch_interrupt(gameboy* gb, uint16_t vec)
{
	write_byte(gb, --gb->SP, (gb->PC >> 8));
	write_byte(gb, --gb->SP, gb->PC & 0xFF);
	gb->PC = vec;
	gb->cycles += 12;
	gb->IME = false;
}
