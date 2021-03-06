#include "types.h"
#include "gb_system.h"
#include "bus.h"
#include "cpu.h"
#include "timers.h"
#include "renderer.h"
#include "ppu.h";
#include "apu.h"
#include "audio.h"
#include "event.h"
#include "apu.h"
#include "mbc/cart.h"
#include "SDL.h"
#include <stdio.h>

gameboy_t gb;

void print_status(FILE* f);

int main(int argc, char** argv) 
{
	bool bootrom = false;
	bool debug = false;

	if (argc < 2)
	{
		printf("Usage: %s <rom.gb>", argv[0]);
		return 1;
	}

	for (int i = 2; i < argc; i++)
	{
		switch (argv[i][1])
		{
			case 'b':
				printf("BOOTROM ON\n");
				bootrom = true;
				break;
			case 'd':
				printf("DEBUG ON\n");
				debug = true;
				break;
		}
	}

	gb_init(argv[1], bootrom);
	renderer_init();
	audio_init();

	FILE* log_file = 0;
	if (debug)
	{
		log_file = fopen("output.log", "w");
	}

	while (quit == false)
	{
		if (debug)
		{
			print_status(log_file);
		}

		cpu_run();
		ppu_run();
		apu_run();
		update_timers();
		
		if (ppu.draw_frame)
		{
			handle_events();
			render();
		}
	}

	cart_write_save();
	
	if (cart.savefile != NULL) 
		fclose(cart.savefile);

	if (debug)
	{
		fclose(log_file);
	}

	SDL_Quit();

	return 0;
}

void print_status(FILE* f)
{
	fprintf(f, "A: %.2X F: %.2X B: %.2X C: %.2X D: %.2X E: %.2X H: %.2X L: %.2X SP: %.4X PC: 00:%.4X (%.2X %.2X %.2X %.2X)\n",
		cpu.r.A, cpu.r.F, cpu.r.B, cpu.r.C, cpu.r.D, cpu.r.E, cpu.r.H, cpu.r.L, cpu.SP, cpu.PC, 
		read_byte(cpu.PC), read_byte(cpu.PC + 1), read_byte(cpu.PC + 2), read_byte(cpu.PC + 3)); gb.cycles -= 16;
}