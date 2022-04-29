#pragma once
#include "types.h"

typedef struct channel
{
	u16					frequency_timer; // aka period aka timer
	u8					envelope_timer;
	u64					last_freq_timer_cycle;
	u8					wave_position;

	u16					frequency;
	u8					volume;
}channel_t;

 //audio regs
typedef union sweep_reg
{
	u8	raw						: 3;
	struct
	{
		u8
			sweep_shift_no		: 3,
			sweep_inc_dec		: 1,
			sweep_time			: 3;
	};
} sweep_reg_t;

typedef union sound_length
{
	u8 raw;						
	struct
	{
		u8
			sound_length_data	: 6,
			wave_pattern_duty	: 2;
	};
} sound_length_t;

typedef union volume_envelope
{
	u8 raw;
	struct
	{
		u8
			period				: 3,
			direction			: 1,
			volume				: 4;
	};
} volume_envelope_t;

typedef union channel_freq
{
	u16 raw;
	struct
	{
		u16
			frequency		: 11,
							: 3,
			counter_select	: 1,
			initial			: 1;
	};
	struct
	{
		u16
			lo				: 8,
			hi				: 8;
	};
} channel_freq_t;

typedef union sound_switch
{
	u8 raw;
	struct
	{
		u8
					: 7,
			on_off	: 1;
	};
} sound_switch_t;

typedef union output_lvl
{
	u8 raw;
	struct
	{
		u8
					: 5,
			volume	: 2;
	};
} output_lvl_t;

typedef union polynomial_calc
{
	u8 raw;
	struct
	{
		u8
			div_ratio		: 3,
			counter_step	: 1,
			shift_clock		: 4;
	};
} polynomial_calc_t;

typedef union counter
{
	u8 raw;
	struct
	{
		u8
					: 6,
			initial	: 1,
			counter	: 1;
	};
} counter_t;

typedef union channel_control
{
	u8 raw;
	struct
	{
		u8
			s01_out_lvl		: 2,
			out_VintoS01	: 1,
			s02_out_lvl		: 2,
			out_VintoS02	: 1;
	};
} channel_control_t;

typedef union sound_out_select
{
	u8 raw;
	struct
	{
		u8
			s1toS01 : 1,
			s2toS01 : 1,
			s3toS01 : 1,
			s4toS01 : 1,
			s1toS02 : 1,
			s2toS02 : 1,
			s3toS02 : 1,
			s4toS02 : 1;
	};
} sound_out_select_t;

// all r/w except bit 7
typedef union sound_onoff
{
	u8 raw;
	struct
	{
		u8
			s1_on : 1,
			s2_on : 1,
			s3_on : 1,
			s4_on : 1,
				  : 3,
			mute  : 1;
	};
} sound_onoff_t;

typedef struct apu
{
	// Channel 1 Tone and Sweep
	channel_t			ch1;
	sweep_reg_t			nr10;
	sound_length_t		nr11;
	volume_envelope_t	nr12;
	channel_freq_t		nr1314;

	// Channel 2 Tone
	channel_t			ch2;
	sound_length_t		nr21;
	volume_envelope_t	nr22;
	channel_freq_t		nr2324;

	// Channel 3 Wave Output
	channel_t			ch3;
	sound_switch_t		nr30;
	u8					nr31;
	output_lvl_t		nr32;
	channel_freq_t		nr3334;
	u8					wave_pattern_ram[16];

	// Channel 4
	channel_t			ch4;
	sound_length_t		nr41;
	volume_envelope_t	nr42;
	polynomial_calc_t	nr43;
	counter_t			nr44;

	// Sound Control Registers
	channel_control_t	nr50;
	sound_out_select_t  nr51;
	sound_onoff_t		nr52;
	u64					frame_sequencer_last_cycle;
	u16					frame_sequencer;

} apu_t;

extern apu_t apu;

apu_run();
u16 get_ch1_freq();
u16 get_ch2_freq();
u16 get_ch3_freq();
u16 get_ch4_freq();
void update_freq_timer(channel_t* channel, u16 frequency);