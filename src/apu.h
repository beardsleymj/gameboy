#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_BUFFER_SIZE 4096

typedef struct 
{
    bool enable;
    bool enable_left;
    bool enable_right;
    bool dac_enable;

    u8 sweep_shift;
    bool sweep_direction;
    u8 sweep_period;
    bool sweep_stop; // ?
    u8 duty;
    u16 length_timer;
	u8 envelope_volume;
    bool envelope_direction;
    u8 envelope_period;
    u8 envelope_period_timer;
    u16 frequency;
    bool counter;
    u16 frequency_timer;

    bool duty_output;
    u8 phase;
    u16 period;

    //these 3 are for sweep
    u8 sweep_timer;
    u16 frequency_shadow;
    bool sweep_enable;

    u8 volume;
    u8 output;
} square1_t;

typedef struct 
{
    bool enable;
    bool enable_left;
    bool enable_right;
    bool dac_enable;
	
    u8 duty;
    u16 length_timer;
    u8 envelope_volume;
    bool envelope_direction;
    u8 envelope_period;
    u8 envelope_period_timer;
    u16 frequency;
    bool counter;
    u16 frequency_timer;

    bool duty_output;
    u8 phase;
    u8 volume;
    u8 output;
} square2_t;

typedef struct 
{
    bool enable;
    bool enable_left;
    bool enable_right;

    bool dac_enable;
    u8 volume;
    u16 frequency;
    bool counter;
    u16 frequency_timer;
    u8 ram[32];
    u8 ram_index;
    u8 pattern;
    
    s16 output;
    u16 length_timer;
} wave_t;

typedef struct 
{
    bool enable;
    bool enable_left;
    bool enable_right;
    bool dac_enable;

    u16 length_timer;
	u8 envelope_volume;
    bool envelope_direction;
    u8 envelope_period;
    u8 envelope_period_timer;
    u16 frequency;
    u16 frequency_timer;
    u8 divisor;
    u8 lfsr_width; // 0 = 15, 1 = 7
    u16 lfsr;
    bool counter;

    u8 output;
    u8 volume;
} noise_t;

typedef struct 
{
    bool left_enable;
    u8 left_volume;
    bool right_enable;
    u8 right_volume;

    bool enable;

    s8 left_sample;
    s8 right_sample;
} sequencer_t;

typedef struct ring_buffer
{
    s8 buffer[SAMPLE_BUFFER_SIZE];
    u16 read_index;
    u16 write_index;
} ring_buffer_t;

typedef struct 
{
    square1_t square1;
    square2_t square2;
    wave_t wave;
    noise_t noise;
    sequencer_t sequencer;
	
    u64 last_frame_sequencer_sync;
    u64 last_sequencer_sync;
    u8 frame_sequencer;

    ring_buffer_t samples;
    u64 cycles;
} apu_t;

extern apu_t apu;

void apu_init();
void apu_run();
void square1_run();
void square2_run();
void wave_run();
void noise_run();
void sequencer_run();
void square1_length();
void square2_length();
void wave_length();
void noise_length();
void square1_sweep();
u8 calculate_new_frequency();
void square1_envelope();
void square2_envelope();
void noise_envelope();
void square1_trigger();
void square2_trigger();
void wave_trigger();
void noise_trigger();

#ifdef __cplusplus
}
#endif