#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMG_CLOCK_FREQ 0x400000
#define DIV_FREQ 0x4000

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
}joypad_t;



typedef struct gameboy 
{
	bool cgb_mode;
	bool IME;
	u8 ime_delay;
	bool halt;
	bool stop;
	u8 bootrom[0x100];
	u8 bootrom_disabled;

	// I/O
	joypad_t joyp;
	u8 sb; // Serial transfer data (R/W)
	u8 sc; // Serial transfer control (R/W)

	// timing
	uint64_t cycles;
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