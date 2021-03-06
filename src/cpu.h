#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union registers
{
	u16 registers[4];
	struct
	{
		u16
			AF,
			BC,
			DE,
			HL;
	};
	struct
	{
		u8
			F, A,
			C, B,
			E, D,
			L, H;
	};
	struct
	{
		u8
			   : 4,
			CF : 1,
			HF : 1,
			NF : 1,
			ZF : 1;
	};
} registers;

typedef union 
{
	u8		raw		: 5;
	struct
	{
		u8
			vblank	: 1,
			stat	: 1,
			timer	: 1,
			serial	: 1,
			joypad	: 1;
	};
} interrupt_enable_t; // interrupt enable

typedef union 
{
	u8		raw		: 5;
	struct
	{
		u8
			vblank	: 1,
			stat	: 1,
			timer	: 1,
			serial	: 1,
			joypad	: 1;
	};

} interrupt_flag_t; // interrupt flag

typedef union tac
{
	u8	raw;
	struct
	{
		u8
			input_clock_select : 2,
			timer_enable : 1;
	};
} tac_t;

typedef struct 
{
	u8 wram[0x8000];
	u8 wram_bank;
	u8 hram[0x7F];
	
	bool double_speed;
	bool prepare_speed_switch;
	registers r;
	u16 SP;
	u16 PC;
	interrupt_enable_t interrupt_enable;
	interrupt_flag_t interrupt_flag;
	u8 div;
	u8 tima;
	u8 tma;
	tac_t tac;
} cpu_t;

extern cpu_t cpu;

void cpu_run();
void handle_interrupts();
void execute_instruction();
void prefix_cb();
u8 cpu_read_wram_byte(u16 address);
void cpu_write_wram_byte(u16 address, u8 value);

#ifdef __cplusplus
}
#endif