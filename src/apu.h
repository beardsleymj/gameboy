#pragma once
#include "gb_system.h"

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

void test_audio();
void SDLAudioCallback(void* userdata, u8* stream, s64 len);
