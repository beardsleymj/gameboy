#include <string.h>
#include <stdlib.h>
#include "ppu.h"
#include "cpu.h"

ppu_t ppu;

u8 ppu_read_byte(u16 address)
{
	return ppu.vram[address & 0x1FFF + (0x2000 * ppu.vram_bank)];
}

void ppu_write_byte(u16 address, u8 value)
{
	ppu.vram[address & 0x1FFF + (0x2000 * ppu.vram_bank)] = value;
}


void ppu_run() 
{	
	while (ppu.cycles <= gb.cycles)	
	{
		switch (ppu.next_mode) 
		{
			case 0:	hblank(); break;
			case 1:	vblank(); break;
			case 2:	oam_search(); break;
			case 3:	draw_scanline(); break;
		}
	}
}

// Mode 0
void hblank() 
{
	ppu.stat.mode_flag = 0;
	if (ppu.ly == 143)
		ppu.next_mode = 1;
	else 
		ppu.next_mode = 2;
	if (ppu.stat.hblank_int_enable)
		cpu.interrupt_flag.stat = 1;
	ppu.cycles += 204;
	increment_ly();
}

// Mode 1
void vblank() 
{
	if (ppu.vblanks == 0) 
	{
		ppu.draw_frame = 1;
		ppu.stat.mode_flag = 1;
		if (ppu.stat.vblank_int_enable)
			cpu.interrupt_flag.stat = 1;
		cpu.interrupt_flag.vblank = 1;
		ppu.vblanks++;
	}
	else 
	{
		increment_ly();
		if (ppu.ly == 154) 
		{
			ppu.next_mode = 2;
			ppu.vblanks = 0;
		}
	}
	ppu.cycles += 456;
}

// Mode 2
void oam_search() 
{
	ppu.stat.mode_flag = 2;
	ppu.next_mode = 3;
	if (ppu.stat.oam_int_enable)
		cpu.interrupt_flag.stat = 1;
	ppu.cycles += 80;

	if (ppu.ly > 143) 
	{
		ppu.window_internal_line_counter = 0;
		ppu.ly = 0;
		ppu.stat.lyc_ly_flag = ppu.ly == ppu.lyc;
		if (ppu.stat.lyc_ly_int_enable && ppu.stat.lyc_ly_flag)
			cpu.interrupt_flag.stat = 1;
	}
	
	ppu.oam_buffer_size = 0;
	for (u16 loc = 0; loc < 0xA0; loc += 4)	
	{
		sprite* current_sprite = (sprite*)&ppu.oam[loc];
		u8 sprite_height = 8 + (ppu.lcdc.obj_size * 8); // lcdc bit 2 for sprite height 0 = 8px, 1 = 16px
		if ((ppu.ly + 16 >= current_sprite->y_pos) && (ppu.ly + 16 < current_sprite->y_pos + sprite_height)) 
		{
			ppu.oam_buffer[ppu.oam_buffer_size++] = *current_sprite;
		}

		if (ppu.oam_buffer_size == 10)
		{
			break;
		}
	}
}

 //Mode 3
 void draw_scanline() 
{
	 ppu.stat.mode_flag = 3;
	 ppu.next_mode = 0;
	 ppu.cycles += 172;

	 u8 scanline[160] = { 0 };

	 if (ppu.lcdc.lcd_enable)
	 {
		 if (ppu.lcdc.bg_win_enable)
			 draw_bg(scanline);

		 if (ppu.lcdc.obj_enable)
			 draw_obj(scanline);
	 }

	 fill_buffer(scanline);
}

void draw_bg(u8* scanline)
{
	u16 line_offset = ((ppu.scx) / 8) & 0x1F;
	u16 window_map_addr = ppu.lcdc.win_tile_map ? 0x1C00 : 0x1800;
	u16 window_map_offset = 0;
	u8 win_tile_col;
	u16 bg_map_addr = ppu.lcdc.bg_tile_map ? 0x1C00 : 0x1800;
	s16 bg_map_offset =  32 * (((ppu.ly + ppu.scy) & 0xFF) / 8);

	u16 tile_col = (ppu.scx) & 7;
	u16 tile_row = 2 * ((ppu.ly + ppu.scy) % 8);

	ppu.window_draw_flag = 0;

	for (u8 i = 0; i < 160; i++)
	{

		u8 tile_number;
		// determines whether to use the bg / window tilemap and gets the tile number
		if (i + 7 >= ppu.wx && ppu.ly >= ppu.wy && ppu.lcdc.win_enable)
		{
			if (ppu.window_draw_flag == 0)
			{
				tile_col = 0;
				tile_row = 2 * (ppu.window_internal_line_counter % 8);
				ppu.window_draw_flag = 1;
			}
			tile_number = ppu.vram[window_map_addr + ((i + 7 - ppu.wx) / 8) + (((ppu.window_internal_line_counter) / 8) * 32)];
		}
		else
		{ // Background
			tile_number = ppu.vram[bg_map_addr + bg_map_offset + line_offset];
		}

		u16 tile_address;
		// start of tile data in vram based on addressing method
		if (ppu.lcdc.tile_data_area) // $8000 method
			tile_address = (tile_number * 16);
		else // $8800 method
			tile_address = 0x1000 + ((s8)tile_number * 16);

		// 1 row of color data
		u8 bg_data1 = ppu.vram[tile_address + tile_row];
		u8 bg_data2 = ppu.vram[tile_address + tile_row + 1];

		u8 color_number = ((bg_data1 >> (7 - tile_col)) & 1) | (((bg_data2 >> (7 - tile_col)) & 1) << 1);

		// Get color using background palette
		u8 color = 0;
		switch (color_number)
		{
			case 0: color = ((ppu.bgp & 0b00000011) >> 0); break;
			case 1: color = ((ppu.bgp & 0b00001100) >> 2); break;
			case 2: color = ((ppu.bgp & 0b00110000) >> 4); break;
			case 3: color = ((ppu.bgp & 0b11000000) >> 6); break;
		}
		
		scanline[i] = color;

		tile_col++;
		tile_col %= 8;
		if (tile_col == 0)
			line_offset = (line_offset + 1) & 31;
	}

	if (ppu.window_draw_flag)
		ppu.window_internal_line_counter++;
}

void draw_obj(u8* scanline)
{
	bool prev_obj_pixel[160] = { 0 }; // keeps track of has been drawn by objs


	for (u8 i = 0; i < ppu.oam_buffer_size; i++)
	{
		sprite* cur_sprite = &ppu.oam_buffer[i];

		// is onscreen
		if (cur_sprite->x_pos - 8 < 0 && cur_sprite->x_pos - 8 > 160)
		{
			continue;
		}

		for (u8 j = 0; j < 8; j++)
		{
			// for double tiles the first bit is zerod
			u8 sprite_tile_number = ppu.lcdc.obj_size ? cur_sprite->tile_number & 0xFE : cur_sprite->tile_number;

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
			u8 sprite_y_offset = 16 - (cur_sprite->y_pos - ppu.ly);

			if (cur_sprite->y_flip)
			{
				if (ppu.lcdc.obj_size)
					sprite_y_offset = (15 - sprite_y_offset);
				else
					sprite_y_offset = (7 - sprite_y_offset);
			}

			u16 sprite_tile_address = (sprite_tile_number * 16);

			u8 sprite_data1 = ppu.vram[sprite_tile_address + (sprite_y_offset * 2)];
			u8 sprite_data2 = ppu.vram[sprite_tile_address + (sprite_y_offset * 2) + 1];

			u8 sprite_color_data = ((sprite_data1 >> (sprite_x_offset)) & 1) | (((sprite_data2 >> (sprite_x_offset)) & 1) << 1);

			u8 sprite_palette;
			
			if (cur_sprite->palette == 1)
				sprite_palette = ppu.obp1;
			else
				sprite_palette = ppu.obp0;

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

		gb.screen_buffer[ppu.ly][i][0] = r;
		gb.screen_buffer[ppu.ly][i][1] = g;
		gb.screen_buffer[ppu.ly][i][2] = b;
	}
}

void increment_ly()
{
	ppu.ly++;
	ppu.stat.lyc_ly_flag = ppu.ly == ppu.lyc;
	if (ppu.stat.lyc_ly_int_enable && ppu.stat.lyc_ly_flag)
		cpu.interrupt_flag.stat = 1;
}

void dma_transfer()
{
	u16 address = ppu.dma * 0x100;
	if (address >= 0xE000)
		address -= 0x2000;

	for (u8 i = 0; i < 0xA0; i++)
	{
		ppu.oam[i] = read_byte(address + i);
	}

	gb.cycles -= 4 * 0xA0;
}

