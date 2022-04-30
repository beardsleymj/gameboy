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

typedef union interrupt_enable
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
} interrupt_enable; // interrupt enable

typedef union interrupt_flag
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

} interrupt_flag; // interrupt flag

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

typedef struct cpu
{
	registers r;
	u16 SP;
	u16 PC;
	interrupt_enable interrupt_enable;
	interrupt_flag interrupt_flag;
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

#ifdef __cplusplus
}
#endif