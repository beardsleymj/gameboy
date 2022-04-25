#include <string.h>
#include <stdlib.h>
#include "ppu.h"

void ppu_run() 
{	
	if (gb.ppu_cycles <= gb.cycles) 
	{
		switch (gb.ppu_next_mode) 
		{
			case 0:	hblank();	break;
			case 1:	vblank();	break;
			case 2:	oam_search();	break;
			case 3:	draw_scanline();	break;
		}
	}
}

// Mode 0
void hblank() 
{
	gb.stat.mode_flag = 0;
	if (gb.ly == 143)
		gb.ppu_next_mode = 1;
	else 
		gb.ppu_next_mode = 2;
	if (gb.stat.hblank_int_enable)
		gb.interrupt_flag.stat = 1;
	gb.ppu_cycles += 204;
	increment_ly();
}

// Mode 1
void vblank() 
{
	if (gb.vblanks == 0) 
	{
		gb.draw_frame = 1;
		gb.stat.mode_flag = 1;
		if (gb.stat.vblank_int_enable)
			gb.interrupt_flag.stat = 1;
		gb.interrupt_flag.vblank = 1;
		gb.vblanks++;
	}
	else 
	{
		increment_ly();
		if (gb.ly == 154) 
		{
			gb.ppu_next_mode = 2;
			gb.vblanks = 0;
		}
	}
	gb.ppu_cycles += 456;
}

// Mode 2
void oam_search() 
{
	gb.stat.mode_flag = 2;
	gb.ppu_next_mode = 3;
	if (gb.stat.oam_int_enable)
		gb.interrupt_flag.stat = 1;
	gb.ppu_cycles += 80;

	if (gb.ly > 143) 
	{
		gb.window_internal_line_counter = 0;
		gb.ly = 0;
		gb.stat.lyc_ly_flag = gb.ly == gb.lyc;
		if (gb.stat.lyc_ly_int_enable && gb.stat.lyc_ly_flag)
			gb.interrupt_flag.stat = 1;
	}
	
	gb.oam_buffer_size = 0;
	for (u16 loc = 0; loc < 0xA0; loc += 4)	
	{
		sprite* current_sprite = &gb.oam[loc];
		u8 sprite_height = 8 + (gb.lcdc.obj_size * 8); // lcdc bit 2 for sprite height 0 = 8px, 1 = 16px
		if ((gb.ly + 16 >= current_sprite->y_pos) && (gb.ly + 16 < current_sprite->y_pos + sprite_height)) 
		{
			gb.oam_buffer[gb.oam_buffer_size++] = *current_sprite;
		}

		if (gb.oam_buffer_size == 10)
		{
			break;
		}
	}
}

 //Mode 3
 void draw_scanline() 
{
	 gb.stat.mode_flag = 3;
	 gb.ppu_next_mode = 0;
	 gb.ppu_cycles += 172;

	 u8 scanline[160] = { 0 };

	 if (gb.lcdc.lcd_enable)
	 {
		 if (gb.lcdc.bg_win_enable)
			 draw_bg(&scanline);

		 if (gb.lcdc.obj_enable)
			 draw_obj(&scanline);
	 }

	 fill_buffer(&scanline);
}

void draw_bg(u8 *scanline)
{
	u16 line_offset = ((gb.scx) / 8) & 0x1F;
	u16 window_map_addr = gb.lcdc.win_tile_map ? 0x1C00 : 0x1800;
	u16 window_map_offset = 0;
	u8 win_tile_col;
	u16 bg_map_addr = gb.lcdc.bg_tile_map ? 0x1C00 : 0x1800;
	s16 bg_map_offset =  32 * (((gb.ly + gb.scy) & 0xFF) / 8);

	u16 tile_col = (gb.scx) & 7;
	u16 tile_row = 2 * ((gb.ly + gb.scy) % 8);

	gb.window_draw_flag = 0;

	for (u8 i = 0; i < 160; i++)
	{

		u8 tile_number;
		// determines whether to use the bg / window tilemap and gets the tile number
		if (i + 7 >= gb.wx && gb.ly >= gb.wy && gb.lcdc.win_enable)
		{
			if (gb.window_draw_flag == 0)
			{
				tile_col = 0;
				tile_row = 2 * (gb.window_internal_line_counter % 8);
				gb.window_draw_flag = 1;
			}
			tile_number = gb.vram[window_map_addr + ((i + 7 - gb.wx) / 8) + (((gb.window_internal_line_counter) / 8) * 32)];
		}
		else
		{ // Background
			tile_number = gb.vram[bg_map_addr + bg_map_offset + line_offset];
		}

		u16 tile_address;
		// start of tile data in vram based on addressing method
		if (gb.lcdc.tile_data_area) // $8000 method
			tile_address = (tile_number * 16);
		else // $8800 method
			tile_address = 0x1000 + ((s8)tile_number * 16);

		// 1 row of color data
		u8 bg_data1 = gb.vram[tile_address + tile_row];
		u8 bg_data2 = gb.vram[tile_address + tile_row + 1];

		u8 color_number = ((bg_data1 >> (7 - tile_col)) & 1) | (((bg_data2 >> (7 - tile_col)) & 1) << 1);

		// Get color using background palette
		u8 color = 0;
		switch (color_number)
		{
			case 0: color = ((gb.bgp & 0b00000011) >> 0); break;
			case 1: color = ((gb.bgp & 0b00001100) >> 2); break;
			case 2: color = ((gb.bgp & 0b00110000) >> 4); break;
			case 3: color = ((gb.bgp & 0b11000000) >> 6); break;
		}
		
		scanline[i] = color;

		tile_col++;
		tile_col %= 8;
		if (tile_col == 0)
			line_offset = (line_offset + 1) & 31;
	}

	if (gb.window_draw_flag)
		gb.window_internal_line_counter++;
}



void draw_obj(u8* scanline)
{
	bool prev_obj_pixel[160] = { 0 }; // keeps track of has been drawn by objs


	for (u8 i = 0; i < gb.oam_buffer_size; i++)
	{
		sprite* cur_sprite = &gb.oam_buffer[i];

		// is onscreen
		if (cur_sprite->x_pos - 8 < 0 && cur_sprite->x_pos - 8 > 160)
		{
			continue;
		}

		for (u8 j = 0; j < 8; j++)
		{
			// for double tiles the first bit is zerod
			u8 sprite_tile_number = gb.lcdc.obj_size ? cur_sprite->tile_number & 0xFE : cur_sprite->tile_number;

			// check if the pixel will be on screen
			if (cur_sprite->x_pos - j - 1 < 0 || cur_sprite->x_pos - j - 1 > 159)
			{
				continue;
			}

			// there is already an obj pixel at this location
			if (prev_obj_pixel[cur_sprite->x_pos - j - 1] == true)
			{
				continue;
			}

			u8 sprite_x_offset = cur_sprite->x_flip ? 7 - j : j;
			u8 sprite_y_offset = 16 - (cur_sprite->y_pos - gb.ly);

			if (cur_sprite->y_flip)
			{
				if (gb.lcdc.obj_size)
					sprite_y_offset = (15 - sprite_y_offset);
				else
					sprite_y_offset = (7 - sprite_y_offset);
			}

			u16 sprite_tile_address = (sprite_tile_number * 16);

			u8 sprite_data1 = gb.vram[sprite_tile_address + (sprite_y_offset * 2)];
			u8 sprite_data2 = gb.vram[sprite_tile_address + (sprite_y_offset * 2) + 1];

			u8 sprite_color_data = ((sprite_data1 >> (sprite_x_offset)) & 1) | (((sprite_data2 >> (sprite_x_offset)) & 1) << 1);

			u8 sprite_palette;
			
			if (cur_sprite->palette == 1)
				sprite_palette = gb.obp1;
			else
				sprite_palette = gb.obp0;

			u8 sprite_color = 0;

			switch (sprite_color_data)
			{
				case 0: continue; // always transparent
				case 1: sprite_color = ((sprite_palette & 0b00001100) >> 2); break;
				case 2: sprite_color = ((sprite_palette & 0b00110000) >> 4); break;
				case 3: sprite_color = ((sprite_palette & 0b11000000) >> 6); break;
			}

			prev_obj_pixel[cur_sprite->x_pos - j - 1] = true;

			if (!(sprite_color_data == 0 || ((cur_sprite->bg_prio && scanline[cur_sprite->x_pos - j - 1]) != 0)))
			{
				scanline[cur_sprite->x_pos - j - 1] = sprite_color;
			}
		}
	}
}


void fill_buffer(u8* scanline)
{
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;

	for (s64 i = 0; i < 160; i++)
	{
		switch (scanline[i])
		{
			case 0:
				r = 0xFF;
				g = 0xFF;
				b = 0xFF;
				break;
			case 1:
				r = 0xAA;
				g = 0xAA;
				b = 0xAA;
				break;
			case 2:
				r = 0x55;
				g = 0x55;
				b = 0x55;
				break;
			case 3:
				r = 0;
				g = 0;
				b = 0;
				break;
		}

		gb.screen_buffer[143 - gb.ly][i][0] = r;
		gb.screen_buffer[143 - gb.ly][i][1] = g;
		gb.screen_buffer[143 - gb.ly][i][2] = b;
	}
}

void increment_ly()
{
	gb.ly++;
	gb.stat.lyc_ly_flag = gb.ly == gb.lyc;
	if (gb.stat.lyc_ly_int_enable && gb.stat.lyc_ly_flag)
		gb.interrupt_flag.stat = 1;
}