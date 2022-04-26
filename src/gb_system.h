#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

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

#define SB 0xFF01
#define SC 0xFF02
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define JOYP 0xFF00
#define DMA 0xFF46
#define OAM_START 0xFE00
#define OAM_END 0xFE9F

typedef union joypad
{
	u8		raw;
	struct
	{
		u8
			right_a		: 1,
			left_b		: 1,
			up_select	: 1,
			down_start	: 1,
			direction	: 1,
			action		: 1;
	};
}joypad;

typedef union tac
{
	u8	raw;
	struct
	{
		u8
			input_clock_select	: 2,
			timer_enable		: 1;
	};
} tac;

typedef struct gameboy 
{
	bool cbg;
	bool IME;
	u8 ime_delay;
	bool halt;
	bool stop;
	u8 bootrom[0x100];
	u8 bootrom_disabled;
	u8 wram[0x2000];	
	u8 oam[0xA0];
	u8 hram[0x7F];

	// I/O RANGES
	joypad joyp;
	u8 sb; // Serial transfer data (R/W)
	u8 sc; // Serial transfer control (R/W)
	u8 div;
	u8 tima;
	u8 tma;
	tac tac;

	u8 nr10;
	u8 audio[0x30]; // probably expand later

	// timing
	uint64_t cycles;
	uint64_t ppu_cycles;
	uint64_t ppu_next_mode;
	uint64_t ppu_line_cycles;
	uint64_t last_sync_cycle;
	uint64_t last_div_cycle;
	uint64_t last_tima_cycle;
	uint32_t input_clock_select[4];

	// input
	bool up;
	bool down;
	bool left;
	bool right;
	bool a;
	bool b;
	bool start;
	bool select;

	u8 screen_buffer[144][160][3];
}gameboy_t;

extern gameboy_t gb;

void gb_init(char* rom_path, bool bootrom);

#ifdef __cplusplus
} 
#endif