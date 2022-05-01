#pragma once
#include "types.h"

typedef struct apu
{
    struct square1
    {
        bool enable;

        bool enable_left;
        bool enable_right;

        u8 sweep_frequency;
        bool sweep_direction;
        u8 sweep_shift;
        bool sweep_stop; // ?
        u8 duty;
        u32 length;
        u8 envelope_volume;
        bool envelope_direction;
        u8 envelope_period;
        u16 frequency;
        bool counter;

        s16 output;
        bool duty_output;
        u8 phase;
        u32 period;
        u8 sweep_period;
        s32 frequency_shadow;
        bool sweep_enable;
        u8 volume;
    } square1;

	struct square2
	{
        bool enable;
        
        bool enable_left;
        bool enable_right;

        u8 duty;
        u32 length;
        u8 envelope_volume;
        bool envelope_direction;
        u8 envelope_frequency;
        u16 frequency;
        bool counter;

        s16 output;
        bool duty_output;
        u8 phase;
        u32 period;
        u8 envelope_period;
        u8 sweep_period;
        s32 frequency_shadow;
        bool sweep_enable;
        u8 volume;
	} square2;

	struct wave
	{
        bool enable;

        bool enable_left;
        bool enable_right;

        bool dac_enable;
        u8 volume;
        u16 frequency;
        bool counter;
        u8 pattern[16];

        s16 output;
        u32 length;
        u32 period;
        u8 pattern_offset;
        u8 pattern_sample;
        u32 pattern_hold;
	} wave;

    struct noise
    {
        bool enable;

        bool enable_left;
        bool enable_right;

        u8 envelope_volume;
        bool envelope_direction;
        u8 envelope_period;
        u8 frequency;
        bool narrow;
        u8 divisor;
        bool counter;

        s16 output;
        u32 length;
        u8 volume;
        u32 period;
        u16 lfsr;
    } noise;

	struct sequencer
	{
        bool left_enable;
        u8 left_volume : 3;
        bool right_enable;
        u8 right_volume : 3;

        //struct Channel {
        //    bool leftEnable;
        //    bool rightEnable;
        //} square1, square2, wave, noise;

        bool enable;

        s16 center;
        s16 left;
        s16 right;
	} sequencer;
} apu_t;

extern apu_t apu;

void apu_run();
void square1_trigger();
void square2_trigger();
void wave_trigger();
void noise_trigger();
u16 get_ch1_freq();
u16 get_ch2_freq();
u16 get_ch3_freq();
u16 get_ch4_freq();
//void update_freq_timer(channel_t* channel, u16 frequency);