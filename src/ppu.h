#pragma once
#include "gb_system.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union sprite
{
	//u8 bytes[4];
	struct
	{
		u8
			y_pos,
			x_pos,
			tile_number,
			sprite_flags;
	};
	struct
	{
		u32
					: 24,
			cgb		: 4,
			palette : 1,
			x_flip	: 1,
			y_flip	: 1,
			bg_prio : 1;
	};
} sprite;

typedef union lcdc
{
	u8		raw;
	struct
	{
		u8
			bg_win_enable	: 1,
			obj_enable		: 1,
			obj_size		: 1,
			bg_tile_map		: 1, // $9C00 or $9800 maps
			tile_data_area	: 1, // $8000 (unsigned) or $8800 (signed) lookup method
			win_enable		: 1,
			win_tile_map	: 1, // $9C00 or $9800 maps
			lcd_enable		: 1;
	};
}lcdc; // lcd control

typedef union stat
{
	u8		raw;
	struct
	{
		u8
			mode_flag			: 2,
			lyc_ly_flag			: 1, // 0 = different, 1 = equal
			hblank_int_enable	: 1, // HBLlank interrupt enable
			vblank_int_enable	: 1, // VBlank interrupt enable
			oam_int_enable		: 1, // OAM interrupt enable
			lyc_ly_int_enable	: 1; // LYC=LY interrupt enable
	};
}stat; // lcd status

typedef struct
{
	// ppu
	u8 vram[0x2000];	// 8000 - 9FFF
	sprite oam_buffer[40];
	u8 oam_buffer_size;
	u8 window_internal_line_counter;
	u8 window_draw_flag;
	u8 vblanks;
	bool draw_frame;

	lcdc lcdc;
	stat stat;
	u8 scy;
	u8 scx;
	u8 ly;
	u8 lyc;
	u8 dma;
	u8 wy;
	u8 wx;
	u8 bgp;
	u8 obp0;
	u8 obp1;
} ppu_t ;

extern ppu_t ppu;

void ppu_run();
void hblank();
void vblank();
void oam_search();
int sprite_compare(sprite*, sprite*);
void draw_scanline();
void increment_ly();
void draw_bg(u8* scanline);
void draw_obj(u8* scanline);
void fill_buffer(u8* scanline);
u8 get_obj_pixel_color(sprite sprt);

#ifdef __cplusplus
}
#endif