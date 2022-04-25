#include <SDL.h>
#include "gb.h"
#include "bus.h"
#include "cpu.h"
#include "timers.h"
#include "renderer.h"
#include "ppu.h";
#include "event.h"
#include "apu.h"
#include "mbc/cart.h"

gameboy_t gb;

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
	test_audio();

	FILE* log_file = 0;
	if (debug)
	{
		log_file = fopen("output.log", "w");
	}
	bool quit = false;
	SDL_Event event;
	while (quit == false)
	{
		if (debug)
		{
			print_status(&log_file);
		}

		cpu_run();
		ppu_run();
		sync_timing();
		if (gb.draw_frame)
		{
			quit = handle_event(&event);
			render();
		}



	}

	if (debug)
	{
		fclose(log_file);
	}
	SDL_Quit();
}

print_status(FILE* f)
{
	fprintf(f, "A: %.2X F: %.2X B: %.2X C: %.2X D: %.2X E: %.2X H: %.2X L: %.2X SP: %.4X PC: 00:%.4X (%.2X %.2X %.2X %.2X)\n", gb.r.A, gb.r.F, gb.r.B, gb.r.C, gb.r.D, gb.r.E, gb.r.H, gb.r.L, gb.SP, gb.PC, read_byte(gb.PC), read_byte(gb.PC + 1), read_byte(gb.PC + 2), read_byte(gb.PC + 3)); gb.cycles -= 16;
	//printf("A: %.2X F: %.2X B: %.2X C: %.2X D: %.2X E: %.2X H: %.2X L: %.2X SP: %.4X PC: 00:%.4X (%.2X %.2X %.2X %.2X)\n", gb.r.A, gb.r.F, gb.r.B, gb.r.C, gb.r.D, gb.r.E, gb.r.H, gb.r.L, gb.SP, gb.PC, read_byte(gb.PC), read_byte(gb.PC + 1), read_byte(gb.PC + 2), read_byte(gb.PC + 3)); gb.cycles -= 16;
}