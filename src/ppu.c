#include <string.h>
#include <stdlib.h>
#include "ppu.h"

void ppu_run(gameboy* gb) 
{	
	if (gb->ppu_cycles <= gb->cycles) 
	{
		switch (gb->ppu_next_mode) 
		{
			case 0:	hblank(gb);	break;
			case 1:	vblank(gb);	break;
			case 2:	oam_search(gb);	break;
			case 3:	draw_scanline(gb);	break;
		}
	}
}

// Mode 0
void hblank(gameboy* gb) 
{
	gb->stat->mode_flag = 0;
	if (gb->mem[LY] == 143)
		gb->ppu_next_mode = 1;
	else 
		gb->ppu_next_mode = 2;
	if (gb->interrupt_enable->stat && gb->stat->hblank_int_enable)
		gb->interrupt_flag->stat = 1;
	gb->ppu_cycles += 204;
	gb->mem[LY]++;
	gb->stat->lyc_ly_flag = gb->mem[LY] == gb->mem[LYC];
	if (gb->interrupt_enable->stat && gb->stat->lyc_ly_int_enable && gb->stat->lyc_ly_flag)
		gb->interrupt_flag->stat = 1;
}

// Mode 1
void vblank(gameboy* gb) 
{
	if (gb->vblanks == 0) 
	{
		gb->draw_frame = 1;
		gb->stat->mode_flag = 1;
		if (gb->interrupt_enable->stat && gb->stat->vblank_int_enable)
			gb->interrupt_flag->stat = 1;
		if (gb->interrupt_enable->vblank)
			gb->interrupt_flag->vblank = 1;
		gb->vblanks++;
	}
	else 
	{
		gb->mem[LY]++;
			gb->stat->lyc_ly_flag = gb->mem[LY] == gb->mem[LYC];
		if (gb->interrupt_enable->stat && gb->stat->lyc_ly_int_enable && gb->stat->lyc_ly_flag)
			gb->interrupt_flag->stat = 1;
		if (gb->mem[LY] == 154) 
		{
			gb->ppu_next_mode = 2;
			gb->vblanks = 0;
		}
	}
	gb->ppu_cycles += 456;
}

// Mode 2
void oam_search(gameboy* gb) 
{
	gb->stat->mode_flag = 2;
	gb->ppu_next_mode = 3;
	if (gb->interrupt_enable->stat && gb->stat->oam_int_enable)
		gb->interrupt_flag->stat = 1;
	gb->ppu_cycles += 80;

	if (gb->mem[LY] > 143) 
	{
		gb->window_internal_line_counter = 0;
		gb->mem[LY] = 0;
		gb->stat->lyc_ly_flag = gb->mem[LY] == gb->mem[LYC];
		if (gb->interrupt_enable->stat && gb->stat->lyc_ly_int_enable && gb->stat->lyc_ly_flag)
			gb->interrupt_flag->stat = 1;
	}
	
	gb->oam_buffer_size = 0;
	for (uint16_t loc = OAM_START; loc < OAM_END; loc += 4)	
	{
		sprite current_sprite;
		memcpy(&current_sprite, &gb->mem[loc], 4);
		uint8_t sprite_height = 8 + (gb->lcdc->obj_size * 8); // lcdc bit 2 for sprite height 0 = 8px, 1 = 16px
		if ((current_sprite.x_pos > 0) && (gb->mem[LY] + 16 >= current_sprite.y_pos) && (gb->mem[LY] + 16 < current_sprite.y_pos + sprite_height) && gb->oam_buffer_size < 10) 
		{
			for (int i = 0; i < gb->oam_buffer_size; i++) 
			{
				if (gb->oam_buffer[i].x_pos == current_sprite.x_pos)
					current_sprite.x_pos = 0;
			}
			gb->oam_buffer[gb->oam_buffer_size++] = current_sprite;
		}
	}
	qsort(&gb->oam_buffer, gb->oam_buffer_size, sizeof(sprite), (*sprite_compare));
}

int sprite_compare(sprite* sprite1, sprite* sprite2) 
{
	return (int16_t)sprite1->x_pos - sprite2->x_pos;
}

// Mode 3
void draw_scanline(gameboy* gb) 
{
	gb->stat->mode_flag = 3;
	gb->ppu_next_mode = 0;
	gb->ppu_cycles += 172;

	uint16_t background_map_address = (gb->lcdc->bg_tile_map) ? 0x9C00 : 0x9800;
	uint16_t background_map_offset = 32 * (((gb->mem[LY] + gb->mem[SCY]) & 0xFF) / 8);
	uint16_t line_offset = ((gb->mem[SCX]) / 8) & 0x1F;
	uint16_t tile_row = 2 * ((gb->mem[LY] + gb->mem[SCY]) % 8);
	uint16_t tile_col = (gb->mem[SCX]) & 7;
	uint16_t window_map_address = (gb->lcdc->win_tile_map) ? 0x9C00 : 0x9800;
	uint16_t window_map_offset = 0;
	uint8_t win_tile_col;
	uint8_t bg_tile_number;
	uint16_t bg_tile_data_offset = (gb->lcdc->tile_data_loc) ? 0x8000 : 0x9000;
	uint16_t bg_tile_address;

	uint8_t oam_pos = 0;
	sprite cur_sprite;
	uint8_t sprite_tile_number;
	uint8_t sprite_tile_data_offset;
	uint16_t sprite_tile_address;
	uint8_t sprite_row = 0;
	uint8_t sprite_col = 0;
	uint8_t sprite_pixels_drawn = 0;

	uint8_t bg_data1;
	uint8_t bg_data2;
	uint8_t sprite_data1;
	uint8_t sprite_data2;

	uint8_t color = 0;
	uint8_t sprite_color;
	uint8_t color_data = 0;
	uint8_t sprite_color_data = 0;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	if (gb->lcdc->win_enable)
		line_offset = 0;

	gb->window_draw_flag = 0;

	for (int16_t i = 0; i < 160; i++) 
	{	
		// BG and Window enable
		if (gb->lcdc->bg_win_prio == 0) 
		{
			red = 0xFF;
			green = 0xff;
			blue = 0xff;
		}
		else 
		{	// window 
			if (i + 7 >= gb->mem[WX] && gb->mem[LY] >= gb->mem[WY] && gb->lcdc->win_enable) 
			{
				if (gb->window_draw_flag == 0) 
				{
					tile_col = 0;
					tile_row = 2 * (gb->window_internal_line_counter % 8);
					gb->window_draw_flag = 1;
				}
				bg_tile_number = gb->mem[window_map_address + ((i + 7 - gb->mem[WX]) / 8) + (((gb->window_internal_line_counter ) / 8) * 32)]; 
			}
			else 
			{ // Background
				bg_tile_number = gb->mem[background_map_address + background_map_offset + line_offset];
			}
				// background / window tile addressing method
			if (gb->lcdc->tile_data_loc) // $8000 method
				bg_tile_address = bg_tile_data_offset + (bg_tile_number * 16);
			else // $8800 method
				bg_tile_address = bg_tile_data_offset + ((int8_t)bg_tile_number * 16);
			
			// get background / window tile data
			bg_data1 = gb->mem[bg_tile_address + tile_row];
			bg_data2 = gb->mem[bg_tile_address + tile_row + 1];

			color_data = ((bg_data1 >> (7 - tile_col)) & 1) | (((bg_data2 >> (7 - tile_col)) & 1) << 1);

			// Get color using background palette
			switch (color_data) 
			{
				case 0: color = ((gb->mem[BGP] & 0b00000011) >> 0); break;
				case 1: color = ((gb->mem[BGP] & 0b00001100) >> 2); break;
				case 2: color = ((gb->mem[BGP] & 0b00110000) >> 4); break;
				case 3: color = ((gb->mem[BGP] & 0b11000000) >> 6); break;
			}

			// sprite
			cur_sprite = gb->oam_buffer[oam_pos];
			int16_t sprite_x_offset = cur_sprite.x_pos - i;
			int16_t sprite_y_offset = 16 - (cur_sprite.y_pos - gb->mem[LY]);
			if (sprite_x_offset >= 0 && sprite_x_offset < 8 && gb->oam_buffer_size != 0) 
			{
				if (sprite_x_offset == 0)
					oam_pos++;
				sprite_tile_number = cur_sprite.tile_number;

				if (gb->lcdc->obj_size) 
				{
					if (gb->mem[LY] + 8 < cur_sprite.y_pos && !cur_sprite.y_flip)
						sprite_tile_number = (sprite_tile_number & 0xFE);
					else
						sprite_tile_number = (sprite_tile_number & 0xFE) + 1;
				}

				sprite_tile_address = 0x8000 + (sprite_tile_number * 16);

				if (cur_sprite.y_flip)
					sprite_y_offset = (7 - sprite_y_offset);
				sprite_data1 = gb->mem[sprite_tile_address + (sprite_y_offset * 2)];
				sprite_data2 = gb->mem[sprite_tile_address + (sprite_y_offset * 2) + 1];

				if (cur_sprite.x_flip)
					sprite_x_offset = 7 - sprite_x_offset;
				
				sprite_color_data = ((sprite_data1 >> (sprite_x_offset)) & 1) | (((sprite_data2 >> (sprite_x_offset)) & 1) << 1);

				uint8_t sprite_palette;
				if (cur_sprite.pallet)
					sprite_palette = gb->mem[OBP1];
				else
					sprite_palette = gb->mem[OBP0];
				switch (sprite_color_data) 
				{
					case 0: sprite_color = ((sprite_palette & 0b00000011) >> 0); break;
					case 1: sprite_color = ((sprite_palette & 0b00001100) >> 2); break;
					case 2: sprite_color = ((sprite_palette & 0b00110000) >> 4); break;
					case 3: sprite_color = ((sprite_palette & 0b11000000) >> 6); break;
				}

				// pixel mixer
				if (((sprite_color && !cur_sprite.bg_prio) || (cur_sprite.bg_prio && color == 0)) && gb->lcdc->obj_enable)
					color = sprite_color;
			}
			else 
			{
				sprite_color = 0x00;
			}
			switch (color) {
				case 0:
					red = 0xFF;
					green = 0xFF;
					blue = 0xFF;
					break;
				case 1:
					red = 0xAA;
					green = 0xAA;
					blue = 0xAA;
					break;
				case 2:
					red = 0x55;
					green = 0x55;
					blue = 0x55;
					break;
				case 3:
					red = 0;
					green = 0;
					blue = 0;
					break;
			}
		}

		gb->screen[143 - gb->mem[LY]][i][0] = red;
		gb->screen[143 - gb->mem[LY]][i][1] = green;
		gb->screen[143 - gb->mem[LY]][i][2] = blue;

		tile_col++;
		tile_col %= 8;
		if (tile_col == 0)
			line_offset = (line_offset + 1) & 31;
	}
	if (gb->window_draw_flag)
		gb->window_internal_line_counter++;
}

