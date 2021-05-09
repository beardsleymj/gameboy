#include <windows.h>
#include "timing.h"

#define TARGET_ELAPSED_NANO 16742706 // 1 000 000 000 (NS/S) / 57.75 (1/S)

uint64_t get_time_nano() 
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return ((uint64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime) * 100;
}

void sleep_nano(uint64_t nanoseconds) 
{
	LARGE_INTEGER lpDueTime;
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	lpDueTime.QuadPart = -(int64_t)(nanoseconds / 100);
	!SetWaitableTimer(hTimer, &lpDueTime, 0, NULL, NULL, 0);
	WaitForSingleObject(hTimer, INFINITE);
}

void sync_timing(gameboy* gb) 
{
	uint64_t time_nano = get_time_nano();
	uint64_t elapsed_nano = time_nano - gb->last_sync_time;
	gb->last_sync_time = time_nano;
	uint64_t elapsed_cycles = gb->cycles - gb->last_sync_cycle;
	gb->last_sync_cycle = gb->cycles;
	uint64_t target_elapsed_time = elapsed_cycles * 1000000000 / DMG_CLOCK_FREQ;
	//sleep_nano(target_elapsed_time - elapsed_nano);
	//uint64_t cycles = gb->cycles - gb->last_sync_cycle;

	sync_div(gb);
	sync_tima(gb);
}


void reset_div(gameboy* gb) 
{
	gb->mem[DIV]= 0;
	gb->last_div_cycle = gb->cycles;
}

void sync_div(gameboy* gb) 
{
	if (gb->cycles - gb->last_div_cycle >= gb->cpu_clock / DIV_FREQ) 
	{
		gb->mem[DIV]++;
		gb->last_div_cycle += gb->cpu_clock / DIV_FREQ;
	}
}

void write_tac(gameboy* gb, uint8_t value) 
{
	gb->mem[TAC] = value;
	sync_tima(gb);
}

#define TIMA_ENABLED 0x04
#define INPUT_CLOCK_SELECT 0x03
void sync_tima(gameboy* gb) 
{
	if (gb->mem[TAC] & (TIMA_ENABLED)) 
	{
		if ((gb->cycles - gb->last_tima_cycle) >= gb->cpu_clock / gb->input_clock_select[gb->mem[TAC] & INPUT_CLOCK_SELECT]) 
		{
			gb->mem[TIMA]++;
			if (gb->mem[TIMA] == 0xFF) 
			{
				gb->mem[TIMA] = gb->mem[TMA];
				if (gb->interrupt_enable->timer)
					gb->interrupt_flag->timer = 1;
			}
			gb->last_tima_cycle += gb->cpu_clock / gb->input_clock_select[gb->mem[TAC] & INPUT_CLOCK_SELECT];
		}
	}
	else 
	{ // TIMA disabled
		gb->last_tima_cycle = gb->cycles;
	}
}