#include <SDL.h>
#include "gb.h"
#include "mem.h"
#include "cpu.h"
#include "timing.h"
#include "renderer.h"
#include "ppu.h";
#include "event.h"

int main(int argc, char** argv) 
{
	if (argc < 2)
	{
		printf("Usage: %s <rom.gb>", argv[0]);
		return 1;
	}

	gameboy* gb = calloc(sizeof(gameboy), 1);

	gb_init(gb, argv[1]);
	renderer_init();

	FILE* log_file = 0;
	log_file = fopen("output.log", "w");
	bool quit = false;
	SDL_Event event;
	int instructions = 0;
	while (quit == false)
	{
		print_status(gb, log_file);
		if (instructions++ % 1000000 == 0)
			printf("%i\n", instructions);
		cpu_run(gb);
		ppu_run(gb);
		sync_timing(gb);
		if (gb->draw_frame == true)
			render(gb);
		while (SDL_PollEvent(&event));
		quit = handle_event(gb, &event);
	}

	fclose(log_file);
	free(gb);
	SDL_Quit;
}

print_status(gameboy* gb, FILE* f)
{
	//fprintf(f, "A: %.2X F: %.2X B: %.2X C: %.2X D: %.2X E: %.2X H: %.2X L: %.2X SP: %.4X PC: 00:%.4X (%.2X %.2X %.2X %.2X)\n", gb->r.A, gb->r.F, gb->r.B, gb->r.C, gb->r.D, gb->r.E, gb->r.H, gb->r.L, gb->SP, gb->PC, read_byte(gb, gb->PC), read_byte(gb, gb->PC + 1), read_byte(gb, gb->PC + 2), read_byte(gb, gb->PC + 3)); gb->cycles -= 16;
	//printf("A: %.2X F: %.2X B: %.2X C: %.2X D: %.2X E: %.2X H: %.2X L: %.2X SP: %.4X PC: 00:%.4X (%.2X %.2X %.2X %.2X)\n", gb->r.A, gb->r.F, gb->r.B, gb->r.C, gb->r.D, gb->r.E, gb->r.H, gb->r.L, gb->SP, gb->PC, read_byte(gb, gb->PC), read_byte(gb, gb->PC + 1), read_byte(gb, gb->PC + 2), read_byte(gb, gb->PC + 3)); gb->cycles -= 16;
}