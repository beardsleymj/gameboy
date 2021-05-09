#include "gb.h"

typedef struct ppu_t
{
	uint8_t screen[144][160][3];
	sprite oam_buffer[40];
	uint8_t oam_buffer_size;
	uint8_t oam_fifo_pos;
	uint8_t oam_fifo_size;
	uint8_t background_fifo_pos;
	uint8_t background_fifo_size;
	uint8_t tile_number;
	uint8_t window_internal_line_counter;
	uint8_t window_draw_flag;
	uint8_t fetcher_y;
	sprite background[32];
	uint8_t vblanks;
	bool draw_frame;
}ppu;

void ppu_run(gameboy* gb);
void hblank(gameboy* gb);
void vblank(gameboy* gb);
void oam_search(gameboy* gb);
int sprite_compare(sprite*, sprite*);
void draw_scanline(gameboy* gb);
