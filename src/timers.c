#include "timers.h"
#include "gb_system.h"
#include "cpu.h"

static const u16 tac_freq[4] = {1024, 16, 64, 256};

void update_timers()
{
	update_div();
	update_tima();
}

void update_div()
{
	while (gb.cycles - gb.last_div_cycle >= 256)
	{
		cpu.div++;
		gb.last_div_cycle += 256;
	}
}

void update_tima()
{
	if (cpu.tac.timer_enable == false)
	{
		return;
	}

	while ((gb.cycles - gb.last_tima_cycle) >= tac_freq[cpu.tac.input_clock_select])
	{
		cpu.tima++;

		if (cpu.tima == 0xFF)
		{
			cpu.tima = cpu.tma;
			cpu.interrupt_flag.timer = 1;
		}

		gb.last_tima_cycle += tac_freq[cpu.tac.input_clock_select];
	}
}

u8 read_div()
{
	//update_div();
	return cpu.div;
}

u8 read_tima()
{
	return cpu.tima;
}

void write_div() 
{
	cpu.div = 0;
	//cpu.tima = 0; // timer uses same internal counter as div
	gb.last_div_cycle = gb.cycles;
	gb.last_tima_cycle = gb.cycles;
}

// Writing to tima resets the entire timer regiester supposedly 
void write_tima(u8 value)
{
	cpu.tima = value;
	cpu.div = 0;
	gb.last_div_cycle = gb.cycles;
	gb.last_tima_cycle = gb.cycles;
}

void write_tac(u8 value) 
{
	// if the timer is enabled, reset the cycles left until interrupt
	if (cpu.tac.timer_enable == false && (value & 0x4) == 0x4)
		gb.last_tima_cycle = gb.cycles;
	cpu.tac.raw = value;
	update_tima();
}



