#pragma once

#include <stdint.h>

typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;
typedef int64_t		s64;
typedef uint64_t	u64;

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
				CF	: 1,
				HF	: 1,
				NF	: 1,
				ZF	: 1;
	};
} registers;

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
			palette	: 1,
			x_flip	: 1,
			y_flip	: 1,
			bg_prio	: 1;
	};
} sprite;

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


// audio regs

//typedef union nr10
//{
//	u8	raw;
//	struct
//	{
//		u8
//			sweep_shift_no		: 3,
//			sweep_inc_dec		: 1,
//			sweep_time			: 3;
//	};
//} nr10;
//
//typedef union nr11
//{
//	u8 raw;
//	struct
//	{
//		u8
//			sound_length_data	: 6,
//			wave_pattern_duty	: 2;
//	};
//} nr11;
//
//typedef union nr12
//{
//	u8 raw;
//	struct
//	{
//		u8
//			no_envelope_sweep	: 3,
//			envelope_direction	: 1,
//			init_vol_envelope	: 4;
//	};
//} nr12;
//
//typedef union nr14
//{
//	u8 
//};