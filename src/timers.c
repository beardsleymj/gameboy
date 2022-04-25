#include "timers.h"

static const u16 tac_freq[4] = {1024, 16, 64, 256};

void sync_timing()
{
	sync_div();
	sync_tima();
}

void sync_div()
{
	while (gb.cycles - gb.last_div_cycle >= 256)
	{
		gb.div++;
		gb.last_div_cycle += 256;
	}
}

void sync_tima()
{
	if (gb.tac.timer_enable == false)
	{
		return;
	}

	while ((gb.cycles - gb.last_tima_cycle) >= tac_freq[gb.tac.input_clock_select])
	{
		gb.tima++;

		if (gb.tima == 0xFF)
		{
			gb.tima = gb.tma;
			gb.interrupt_flag.timer = 1;
		}

		gb.last_tima_cycle += tac_freq[gb.tac.input_clock_select];
	}
}

u8 read_div()
{
	//sync_div();
	return gb.div;
}

void write_div() 
{
	gb.div = 0;
	gb.tima = 0; // timer uses same internal counter as div
	gb.last_div_cycle = gb.cycles;
	gb.last_tima_cycle = gb.cycles;
}

void write_tac(u8 value) 
{
	// if the timer is enabled, reset the cycles left until interrupt
	if (gb.tac.timer_enable == false && (value & 0x4) == 0x4)
		gb.last_tima_cycle = gb.cycles;
	gb.tac.raw = value;
	sync_tima();
}

u8 read_tima()
{
	return gb.tima;
}

// Writing to tima resets the entire timer regiester supposedly 
u8 write_tima(u8 value)
{
	gb.tima = value;
	gb.div = 0;
	gb.last_div_cycle = gb.cycles;
	gb.last_tima_cycle = gb.cycles;
}