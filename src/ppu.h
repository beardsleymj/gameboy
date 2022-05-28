#pragma once
#include "gb_system.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union
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
			palette_number	: 3,
			vram_bank		: 1,
			palette			: 1,
			x_flip			: 1,
			y_flip			: 1,
			bg_prio			: 1;
	};
} sprite_t;

typedef union
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
} lcdc_t; // lcd control

typedef union 
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
} stat_t; // lcd status

typedef union
{
	u16 raw;
	struct
	{
		u16
			r : 5,
			g : 5,
			b : 5,
			  : 1;
	};
} palette_data_t;

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} rgb888_t;

typedef struct
{
	u8
		background_palette_number : 3,
		tile_vram_bank_number	  : 1,
								  : 1,
		horizontal_flip			  : 1,
		vertical_flip             : 1,
		bg_to_oam_priority        : 1;
} bg_map_attributes_t;

typedef struct
{
	uint64_t cycles;
	uint64_t next_mode;
	u8 vram[2][0x2000];
	u8 oam[0xA0];
	sprite_t oam_buffer[40];
	u8 oam_buffer_size;
	bool wy_ly_flag;
	s16 window_internal_line_counter;
	u8 window_draw_flag;
	bool draw_frame;

	lcdc_t lcdc;
	stat_t stat;
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

	// cgb
	u8 bgpi; // background palette index
	u8 bgpi_auto_inc;
	u8 obpi;
	u8 obpi_auto_inc;
	u8 bgpd[64];
	u8 obpd[64];
	u16 hdma_source;
	u16 hdma_destination;
	u8 hdma_length;
	u16 hdma_bytes_transferred;
	u16 transfer_length;
	u8 hdma_mode; // 0 - general purpose, 1 - hblank
	u8 hdma_transfer_active;
	u8 obj_prio_mode;
	u8 vram_bank;
	u8 bg_master_prio[160];

	rgb888_t dmg_colors[4];
} ppu_t ;

extern ppu_t ppu;

u8 ppu_read_byte(u16 address);
void ppu_write_byte(u16 address, u8 value);
void ppu_run();
void hblank();
void vblank();
void oam_search();
int sprite_compare(const sprite_t*, const sprite_t*);
void draw_scanline();
void increment_ly();
void draw_bg(u8* scanline);
void draw_obj(u8* scanline);
void dma_transfer();
void hdma_transfer();
void gdma_transfer();
#ifdef __cplusplus
}
#endif