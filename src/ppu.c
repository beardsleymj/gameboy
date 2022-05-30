#include <string.h>
#include <stdlib.h>
#include "stdlib.h"
#include "ppu.h"
#include "cpu.h"
#include "bus.h"

ppu_t ppu;

u8 ppu_read_byte(u16 address)
{
	return ppu.vram[ppu.vram_bank][address & 0x1FFF];
}

void ppu_write_byte(u16 address, u8 value)
{
	ppu.vram[ppu.vram_bank][address & 0x1FFF] = value;
}


void ppu_run() 
{	
	if (!ppu.lcdc.lcd_enable)
	{
		ppu.cycles = gb.cycles;
		return;
	}
	
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
	ppu.cycles += 207 * (cpu.double_speed + 1);

	if (ppu.hdma_transfer_active)
		hdma_transfer();

	ppu.stat.mode_flag = 0;

	if (ppu.ly == 143)
		ppu.next_mode = 1;
	else 
		ppu.next_mode = 2;

	if (ppu.stat.hblank_int_enable)
		cpu.interrupt_flag.stat = 1;
	
}

// Mode 1
void vblank() 
{
	ppu.cycles += 456 * (cpu.double_speed + 1);

	increment_ly();

	if (ppu.ly == 144) 
	{
		ppu.wy_ly_flag = 0;
		ppu.draw_frame = 1;
		ppu.stat.mode_flag = 1;
		
		if (ppu.stat.vblank_int_enable)
			cpu.interrupt_flag.stat = 1;
		
		cpu.interrupt_flag.vblank = 1;
	}

	if (ppu.ly == 153) 
	{
		ppu.next_mode = 2;
	}
}



// Mode 2
void oam_search() 
{
	ppu.cycles += 80 * (cpu.double_speed + 1);

	increment_ly();
	ppu.stat.mode_flag = 2;
	ppu.next_mode = 3;

	if (ppu.stat.oam_int_enable)
		cpu.interrupt_flag.stat = 1;
	
	ppu.oam_buffer_size = 0;
	for (u16 loc = 0; loc < 0xA0; loc += 4)	
	{
		sprite_t* current_sprite = (sprite_t*)&ppu.oam[loc];
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
	
	// sort so further left objects are on top for cgb
	if (!gb.cgb_mode)
	{
		qsort(ppu.oam_buffer, ppu.oam_buffer_size, sizeof(sprite_t), sprite_compare);
	}
}

int sprite_compare(const sprite_t* a, const sprite_t* b)
{	
	return a->x_pos - b->x_pos + 1;
}

 //Mode 3
 void draw_scanline() 
{
	 ppu.cycles += 175 * (cpu.double_speed + 1);
	 ppu.stat.mode_flag = 3;
	 ppu.next_mode = 0;
	
	 u8 scanline[160] = { 0 };
	 if (gb.cgb_mode || ppu.lcdc.bg_win_enable)
		 draw_bg(scanline);
	 if (ppu.lcdc.obj_enable)
		 draw_obj(scanline);
}

// this is a really stupid / slow way to do the bg / window. should probably be drawing per tile instead of per pixel, doing the background first then the window on top
// because we are recalcualating things so often
void draw_bg(u8* scanline)
{
	u16 line_offset = ((ppu.scx) / 8) & 0x1F;
	u16 window_map_addr = ppu.lcdc.win_tile_map ? 0x1C00 : 0x1800;
	u16 bg_map_addr = ppu.lcdc.bg_tile_map ? 0x1C00 : 0x1800;
	s16 bg_map_offset =  32 * (((ppu.ly + ppu.scy) & 0xFF) / 8);

	u16 tile_col = (ppu.scx) & 7;
	u16 tile_row = 2 * ((ppu.ly + ppu.scy) % 8);

	ppu.window_draw_flag = 0;

	// One of the conditions to draw the window is for ly to have been equal to wy at some point
	if (ppu.wy == ppu.ly)
		ppu.wy_ly_flag = 1;

	for (u8 i = 0; i < 160; i++)
	{
		u16 tile_map_address;
		u8 tile_number;
		// determines whether to use the bg / window tilemap and gets the tile number
		if (i + 7 >= ppu.wx && ppu.wy_ly_flag && ppu.lcdc.win_enable)
		{
			if (ppu.window_draw_flag == 0)
			{
				tile_col = 0;
				tile_row = 2 * (ppu.window_internal_line_counter % 8);
				ppu.window_draw_flag = 1;
			}
			tile_map_address = window_map_addr + ((i + 7 - ppu.wx) / 8) + (((ppu.window_internal_line_counter) / 8) * 32);
			tile_number = ppu.vram[0][tile_map_address];

		}
		else
		{ // Background
			tile_map_address = bg_map_addr + bg_map_offset + line_offset;
			tile_number = ppu.vram[0][tile_map_address];
		}
		
		bg_map_attributes_t tile_attributes = *(bg_map_attributes_t*)&ppu.vram[1][tile_map_address];

		// 8000 or 8800 method
		u16 tile_address = ppu.lcdc.tile_data_area ? tile_number * 16 : 0x1000 + (s8)tile_number * 16;
		
		u16 row = tile_row;
		if (tile_attributes.vertical_flip)
			row = 14 - row;

		u8 bank = gb.cgb_mode && tile_attributes.tile_vram_bank_number;
		u8 bg_data1 = ppu.vram[bank][tile_address + row];
		u8 bg_data2 = ppu.vram[bank][tile_address + row + 1];

		u16 col = tile_col;
		if (tile_attributes.horizontal_flip)
			col = 7 - col;
		u8 color_number = ((bg_data1 >> (7 - col)) & 1) | (((bg_data2 >> (7 - col)) & 1) << 1);

		palette_data_t palette;
		rgb888_t color;
		if (gb.cgb_mode)
		{
			palette = *(palette_data_t*)&ppu.bgpd[tile_attributes.background_palette_number * 8 + color_number * 2];
			color.r = (palette.r << 3) | (palette.r >> 2);
			color.g = (palette.g << 3) | (palette.g >> 2);
			color.b = (palette.b << 3) | (palette.b >> 2);
			scanline[i] = color_number;
			ppu.bg_master_prio[i] = tile_attributes.bg_to_oam_priority && color_number != 0;
		}
		else
		{
			u8 dmg_color_index = 0;
			switch (color_number)
			{
				case 0: dmg_color_index = ((ppu.bgp & 0b00000011) >> 0); break;
				case 1: dmg_color_index = ((ppu.bgp & 0b00001100) >> 2); break;
				case 2: dmg_color_index = ((ppu.bgp & 0b00110000) >> 4); break;
				case 3: dmg_color_index = ((ppu.bgp & 0b11000000) >> 6); break;
			}
			color = ppu.dmg_colors[dmg_color_index];
			scanline[i] = dmg_color_index != 0;
		}

		gb.screen_buffer[ppu.ly][i][0] = color.r;
		gb.screen_buffer[ppu.ly][i][1] = color.g;
		gb.screen_buffer[ppu.ly][i][2] = color.b;

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
	bool prev_obj_pixel_placed[160] = { 0 }; 

	for (u8 i = 0; i < ppu.oam_buffer_size; i++)
	{
		sprite_t* sprite = &ppu.oam_buffer[i];

		// is onscreen
		if (sprite->x_pos - 8 < 0 && sprite->x_pos - 8 > 160)
		{
			continue;
		}

		for (u8 j = 0; j < 8; j++)
		{
			// there is already an obj pixel at this location
			if (prev_obj_pixel_placed[sprite->x_pos - j - 1] == true)
			{
				continue;
			}

			// check if the pixel will be on screen
			if (sprite->x_pos - j - 1 < 0 || sprite->x_pos - j - 1 > 159)
			{
				continue;
			}

			u8 sprite_x_offset = sprite->x_flip ? 7 - j : j;
			u8 sprite_y_offset = 16 - (sprite->y_pos - ppu.ly);

			if (sprite->y_flip)
			{
				if (ppu.lcdc.obj_size)
					sprite_y_offset = (15 - sprite_y_offset);
				else
					sprite_y_offset = (7 - sprite_y_offset);
			}

			// for double tiles the first bit is zerod
			u8 sprite_tile_number = ppu.lcdc.obj_size ? sprite->tile_number & 0xFE : sprite->tile_number;
			u16 sprite_tile_address = (sprite_tile_number * 16);

			u8 bank = gb.cgb_mode ? sprite->vram_bank : 0;
			u8 sprite_data1 = ppu.vram[bank][sprite_tile_address + (sprite_y_offset * 2)];
			u8 sprite_data2 = ppu.vram[bank][sprite_tile_address + (sprite_y_offset * 2) + 1];

			u8 sprite_color_data = ((sprite_data1 >> (sprite_x_offset)) & 1) | (((sprite_data2 >> (sprite_x_offset)) & 1) << 1);

			rgb888_t color;
			if (gb.cgb_mode) 
			{
				palette_data_t rgb555 = *(palette_data_t*)&ppu.obpd[sprite->palette_number * 8 + sprite_color_data * 2];
				color.r = (rgb555.r << 3) | (rgb555.r >> 2);
				color.g = (rgb555.g << 3) | (rgb555.g >> 2);
				color.b = (rgb555.b << 3) | (rgb555.b >> 2);
			}
			else // in theory gbc should always use this i believe, we need to write palettes to obpd at start up that would be written by the boot rom
			{
				u8 sprite_palette = sprite->palette ? ppu.obp1 : ppu.obp0;
				u8 sprite_color = 0;
				switch (sprite_color_data)
				{
					case 0: continue; // always transparent
					case 1: sprite_color = ((sprite_palette & 0b00001100) >> 2); break;
					case 2: sprite_color = ((sprite_palette & 0b00110000) >> 4); break;
					case 3: sprite_color = ((sprite_palette & 0b11000000) >> 6); break;
				}
				color = ppu.dmg_colors[sprite_color];
			}

			if (gb.cgb_mode && ppu.lcdc.bg_win_enable && ppu.bg_master_prio[sprite->x_pos - j - 1])
			{
				continue;
			}
			
			if (!(sprite_color_data == 0 || (sprite->bg_prio && scanline[sprite->x_pos - j - 1]) != 0))
			{
				prev_obj_pixel_placed[sprite->x_pos - j - 1] = true;
				gb.screen_buffer[ppu.ly][sprite->x_pos - j - 1][0] = color.r;
				gb.screen_buffer[ppu.ly][sprite->x_pos - j - 1][1] = color.g;
				gb.screen_buffer[ppu.ly][sprite->x_pos - j - 1][2] = color.b;
			}
		}
	}
}

void increment_ly()
{
	ppu.ly++;
	if (ppu.ly == 154)
	{
		ppu.ly = 0;
		ppu.window_internal_line_counter = 0;
	}
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

void hdma_transfer()
{
	for (int i = 0; i < 0x10; i++)
	{
		ppu_write_byte(ppu.hdma_destination + ppu.hdma_bytes_transferred + i, read_byte(ppu.hdma_source + ppu.hdma_bytes_transferred + i));
		gb.cycles -= 4;
	}
	// TODO: Figure out timing stuff
	gb.cycles += 32;
	ppu.hdma_bytes_transferred += 0x10;
	ppu.hdma_length--;
	ppu.hdma_transfer_active = ppu.hdma_bytes_transferred != ppu.transfer_length;
}

void gdma_transfer()
{
	for (int i = 0; i < ppu.transfer_length; i++)
	{
		ppu_write_byte(ppu.hdma_destination + i, read_byte(ppu.hdma_source + i));
		gb.cycles -= 4;
	}
	// TODO: figure out timing stuff
	gb.cycles += 32 * (ppu.transfer_length + 1);
}