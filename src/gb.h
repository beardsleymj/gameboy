#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define DMG_CLOCK_FREQ 0x400000
#define DIV_FREQ 0x4000

#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define WY 0xFF4A
#define WX 0xFF4B
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49

#define INTERRUPT_ENABLE 0xFFFF
#define INTERRUPT_FLAG 0xFF0F

#define VBLANK 0x01
#define LCD_STAT 0x02
#define TIMER 0x04
#define SERIAL 0x08
#define JOYPAD 0X10

#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define JOYP 0xFF00
#define DMA 0xFF46
#define OAM_START 0xFE00
#define OAM_END 0xFE9F



typedef union registers 
{
	uint16_t registers[4];
	struct
	{
		uint16_t AF,
			 	 BC,
			 	 DE,
			     HL;
	};
	struct
	{
		// Might need to change for other endianness
		uint8_t F, A,
				C, B,
				E, D,
				L, H;
	};
	struct
	{
		uint8_t   
					: 4,
				CF	: 1,
				HF	: 1,
				NF	: 1,
				ZF	: 1;
	};
} registers;

typedef union sprite
{
	//uint8_t bytes[4];
	struct
	{
		uint8_t	y_pos,
				x_pos,
				tile_number,
				sprite_flags;
	};
	struct
	{
		uint32_t
					: 24,
			cgb		: 4,
			pallet	: 1,
			x_flip	: 1,
			y_flip	: 1,
			bg_prio	: 1;
	};
} sprite;

typedef struct tile_byte
{
	uint8_t
		bit7 : 1,
		bit6 : 1,
		bit5 : 1,
		bit4 : 1,
		bit3 : 1,
		bit2 : 1,
		bit1 : 1,
		bit0 : 1;
} tile_byte;

typedef struct interrupt_enable
{
	uint8_t
		vblank	: 1,
		stat	: 1,
		timer	: 1,
		serial	: 1,
		joypad	: 1;
}interrupt_enable; // interrupt enable

typedef struct interrupt_flag
{
	uint8_t
		vblank	: 1,
		stat	: 1,
		timer	: 1,
		serial	: 1,
		joypad	: 1;
} interrupt_flag; // interrupt flag

typedef struct lcdc
{
	uint8_t
		bg_win_prio		: 1,
		obj_enable		: 1,
		obj_size		: 1,
		bg_tile_map		: 1,
		tile_data_loc	: 1,
		win_enable		: 1,
		win_tile_map	: 1,
		lcd_enable		: 1;

}lcdc; // lcd control

typedef struct stat
{
	uint8_t
		mode_flag			: 2,
		lyc_ly_flag			: 1, // 0 = different, 1 = equal
		hblank_int_enable	: 1, // HBLlank interrupt enable
		vblank_int_enable	: 1, // VBlank interrupt enable
		oam_int_enable		: 1, // OAM interrupt enable
		lyc_ly_int_enable	: 1; // LYC=LY interrupt enable
}stat; // lcd status

typedef struct joypad
{
	uint8_t
		right_a		: 1,
		left_b		: 1,
		up_select	: 1,
		down_start	: 1,
		direction	: 1,
		action		: 1,
		unused		: 2;
}joypad;

typedef enum banking_modes { simple, advanced } banking_modes;

typedef struct gameboy 
{
	uint64_t cpu_clock;
	registers r;
	uint16_t SP;
	uint16_t PC;
	bool IME;
	uint8_t ime_delay;
	bool halt;
	bool stop;
	bool dma_transfer;
	uint8_t bootrom[0x100];
	uint8_t bootrom_disabled;
	uint8_t vram[0x2000];
	uint8_t wram1[0x1000];
	uint8_t wram2[0x1000];
	uint8_t oam[0x100];
	uint8_t hram[0x80];
	uint8_t mem[0x10000];
	interrupt_enable* interrupt_enable;
	interrupt_flag* interrupt_flag;
	lcdc* lcdc;
	stat* stat;
	


	// timing
	uint64_t last_sync_time;
	uint64_t last_sync_cycles;
	uint64_t cycles;
	uint64_t ppu_cycles;
	uint64_t ppu_next_mode;
	uint64_t ppu_line_cycles;
	uint64_t last_sync_cycle;
	uint64_t last_div_cycle;
	uint64_t last_tima_cycle;
	uint32_t input_clock_select[4];

	// ppu
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

	// cartridge
	uint8_t* rom;
	uint16_t rom_banks;
	uint8_t* sram;
	uint8_t ram_banks;
	char title[16];
	uint8_t cgb_flag;
	uint8_t cart_type;
	uint16_t rom_address_pins_mask;
	uint8_t bank1_reg;
	uint8_t bank2_reg;
	uint8_t ram_bank_enable;
	banking_modes banking_mode;

	// input
	joypad* joypad;
	bool up;
	bool down;
	bool left;
	bool right;
	bool a;
	bool b;
	bool start;
	bool select;
}gameboy;

void gb_init(gameboy* gb, char* rom_path);
