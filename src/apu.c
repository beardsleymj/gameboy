#include "types.h"
#include "apu.h"
#include "gb_system.h"
#include "audio.h"

#include <stdio.h>

const u8 wave_duty_table[32] =
{
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 0
};

apu_t apu;

void apu_init()
{


}

void apu_run()
{
	// sync apu with cpu, apu runs at half cpu speed
	while (gb.cycles >= apu.cycles)
	{
		apu.cycles++;
		
		square1_run();
		square2_run();
		wave_run();
		noise_run();
	}
	
	// update frame sequencer
	while (gb.cycles - apu.last_frame_sequencer_sync >= 8192)
	{
		apu.last_frame_sequencer_sync += 8192;
		apu.frame_sequencer = apu.frame_sequencer + 1;
		apu.frame_sequencer &= 0b111;

		// clock stuff based on frame sequencer
		switch (apu.frame_sequencer)
		{
			case 0:
				square1_length();
				square2_length();
				wave_length();
				noise_length();
				break;
			case 1:

				break;

			case 2:
				square1_length();
				square2_length();
				wave_length();
				noise_length();

				square1_sweep();
				break;

			case 3:

				break;

			case 4:
				square1_length();
				square2_length();
				wave_length();
				noise_length();
				break;

			case 5:

				break;

			case 6:
				square1_length();
				square2_length();
				wave_length();
				noise_length();

				square1_sweep();
				break;

			case 7:
				square1_envelope();
				square2_envelope();
				noise_envelope();
				break;
		}
	}

	while (gb.cycles - apu.last_sequencer_sync > DMG_CLOCK_FREQ / SAMPLE_RATE)
	{
		apu.last_sequencer_sync += DMG_CLOCK_FREQ / SAMPLE_RATE;
		sequencer_run();
		audio_queue_sample(&apu.sequencer.left_sample);
		audio_queue_sample(&apu.sequencer.right_sample);
	}
}

void square1_run()
{
	apu.square1.frequency_timer--;
	if (apu.square1.frequency_timer == 0)
	{
		apu.square1.frequency_timer = (2048 - apu.square1.frequency) * 4;
		apu.square1.phase++;
		apu.square1.phase &= 0b111;
		apu.square1.duty_output = wave_duty_table[apu.square1.duty * 8 + apu.square1.phase];
	}

	u8 sample = apu.square1.duty_output * apu.square1.volume * apu.square1.enable;

	apu.square1.output = sample;
}

void square2_run()
{
	apu.square2.frequency_timer--;
	if (apu.square2.frequency_timer == 0) 
	{
		apu.square2.frequency_timer = (2048 - apu.square2.frequency) * 4;
		apu.square2.phase++;
		apu.square2.phase &= 0b111;
		apu.square2.duty_output = wave_duty_table[apu.square2.duty * 8 + apu.square2.phase];
	}

	u8 sample = apu.square2.duty_output * apu.square2.volume * apu.square2.enable;
	
	apu.square2.output = sample;
}

const u32 shift[] = { 4, 0, 1, 2 }; // 0%, 100%, 50% 25%
void wave_run()
{
	apu.wave.frequency_timer--;	
	if (apu.wave.frequency_timer == 0)
	{
		apu.wave.pattern = apu.wave.ram[apu.wave.ram_index++];
		apu.wave.ram_index = apu.wave.ram_index & 0x1F;
		apu.wave.frequency_timer = (2048 - apu.wave.frequency) * 2;
	}

	u8 sample = apu.wave.pattern >> shift[apu.wave.volume];
	if (!apu.wave.enable) sample = 0;
	apu.wave.output = sample;
}

const u8 divisor[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };
void noise_run()
{
	apu.noise.frequency_timer--;
	if (apu.noise.frequency_timer == 0)
	{
		apu.noise.frequency_timer = divisor[apu.noise.divisor] << apu.noise.frequency;
		
		u16 result = (apu.noise.lfsr & 0b01) ^ ((apu.noise.lfsr & 0b10) >> 1);
		apu.noise.lfsr = (apu.noise.lfsr >> 1) | (result << 14);

		if (apu.noise.lfsr_width)
		{
			apu.noise.lfsr &= !(1 << 6);
			apu.noise.lfsr |= result << 6;
		}
	}
	
	u8 sample = apu.noise.lfsr & 1;
	sample = sample * apu.noise.volume;
	if (!apu.noise.enable) sample = 0;

	apu.noise.output = sample;
}

void square1_length()
{
	if (apu.square1.counter && apu.square1.length_timer > 0)
	{
		apu.square1.length_timer--;
		if (apu.square1.length_timer == 0)
			apu.square1.enable = false;
	}
}

void square2_length()
{
	if (apu.square2.counter && apu.square2.length_timer > 0)
	{
		apu.square2.length_timer--;
		if (apu.square2.length_timer == 0)
			apu.square2.enable = false;
	}
}

void wave_length()
{
	if (apu.wave.counter && apu.wave.length_timer > 0)
	{
		apu.wave.length_timer--;
		if (apu.wave.length_timer == 0)
			apu.wave.enable = false;
	}
}

void noise_length()
{
	if (apu.noise.counter && apu.noise.length_timer > 0)
	{
		apu.noise.length_timer--;
		if (apu.noise.length_timer == 0)
			apu.noise.enable = false;
	}
}

void square1_sweep()
{
	if (apu.square1.sweep_timer > 0)
	{
		apu.square1.sweep_timer--;
	}

	if (apu.square1.sweep_timer == 0)
	{
		if (apu.square1.sweep_period > 0)
		{
			apu.square1.sweep_timer = apu.square1.sweep_period;
		}
		else
		{
			apu.square1.sweep_timer = 8;
		}

		if (apu.square1.sweep_enable && apu.square1.sweep_period > 0)
		{
			u8 new_frequency = calculate_new_frequency();
			
			if (new_frequency <= 2047 && apu.square1.sweep_shift > 0)
			{
				apu.square1.frequency = new_frequency;
				apu.square1.frequency_shadow = new_frequency;
			}
			
			// for overflow check
			calculate_new_frequency();
		}
	}
}

u8 calculate_new_frequency()
{
	u8 new_frequency = apu.square1.frequency_shadow >> apu.square1.sweep_shift;


	// 0 = add, 1 = subtract
	if (apu.square1.sweep_direction)
	{
		new_frequency = apu.square1.frequency_shadow - new_frequency;
	}
	else
	{
		new_frequency = apu.square1.frequency_shadow + new_frequency;
	}

	// for overflow check
	if (new_frequency > 2047)
	{
		apu.square1.enable = false;
	}

	return new_frequency;
}

void square1_envelope()
{
	if (apu.square1.envelope_period == 0)
		return;

	if (apu.square1.envelope_period_timer > 0)
		apu.square1.envelope_period_timer--;

	if (apu.square1.envelope_period_timer == 0)
	{
		apu.square1.envelope_period_timer = apu.square1.envelope_period ? apu.square1.envelope_period : 8;

		if (apu.square1.volume < 0xF && apu.square1.envelope_direction == 1)
			apu.square1.volume++;

		if (apu.square1.volume > 0 && apu.square1.envelope_direction == 0)
			apu.square1.volume--;
	}
}

void square2_envelope()
{
	if (apu.square2.envelope_period == 0)
		return;
	
	if (apu.square2.envelope_period_timer > 0)
		apu.square2.envelope_period_timer--;
	
	if (apu.square2.envelope_period_timer == 0)
	{
		apu.square2.envelope_period_timer = apu.square2.envelope_period ? apu.square2.envelope_period : 8;

		if (apu.square2.volume < 0xF && apu.square2.envelope_direction == 1)
			apu.square2.volume++;
		
		if (apu.square2.volume > 0 && apu.square2.envelope_direction == 0)
			apu.square2.volume--;
	}
}

void noise_envelope()
{
	if (apu.noise.envelope_period == 0)
		return;

	if (apu.noise.envelope_period_timer > 0)
		apu.noise.envelope_period_timer--;

	if (apu.noise.envelope_period_timer == 0)
	{
		apu.noise.envelope_period_timer = apu.noise.envelope_period ? apu.noise.envelope_period : 8;

		if (apu.noise.volume < 0xF && apu.noise.envelope_direction == 1)
			apu.noise.volume++;

		if (apu.noise.volume > 0 && apu.noise.envelope_direction == 0)
			apu.noise.volume--;
	}
}

void square1_trigger()
{
	apu.square1.enable = apu.square1.dac_enable;
	apu.square1.frequency_timer = (2048 - apu.square1.frequency) * 4;
	apu.square1.envelope_period_timer = apu.square1.envelope_period ? apu.square1.envelope_period : 8;
	apu.square1.volume = apu.square1.envelope_volume;
	if (apu.square1.length_timer == 0) apu.square1.length_timer = 64;	
	
	apu.square1.frequency_shadow = apu.square1.frequency;
	
	apu.square1.sweep_timer = apu.square1.sweep_period;
	if (apu.square1.sweep_timer == 0)
		apu.square1.sweep_timer = 8;
		
	if (apu.square1.sweep_period != 0 || apu.square1.sweep_shift != 0)
		apu.square1.sweep_enable = true;

	
	// for overflow check
	if (apu.square1.sweep_shift != 0)
		calculate_new_frequency();
}

void square2_trigger()
{
	apu.square2.enable = apu.square2.dac_enable;
	apu.square2.frequency_timer = (2048 - apu.square2.frequency) * 4;
	apu.square2.envelope_period_timer = apu.square2.envelope_period ? apu.square2.envelope_period : 8;
	apu.square2.volume = apu.square2.envelope_volume;
	if (apu.square2.length_timer == 0) apu.square2.length_timer = 64;
}

void wave_trigger()
{
	apu.wave.enable = apu.wave.dac_enable;
	apu.wave.frequency_timer = (2048 - apu.wave.frequency) * 2;
	apu.wave.ram_index = 0;
	apu.wave.pattern = 0;
}

//const u8 divisor[8] = {8, 16, 32, 48, 64, 80, 96, 112};
void noise_trigger()
{
	apu.noise.enable = apu.noise.dac_enable;
	apu.noise.frequency_timer = divisor[apu.noise.divisor] << apu.noise.frequency;
	apu.noise.envelope_period_timer = apu.noise.envelope_period ? apu.noise.envelope_period : 8;
	apu.noise.volume = apu.noise.envelope_volume;
	if (apu.noise.length_timer == 0) apu.noise.length_timer = 64;
	apu.noise.lfsr = 0x7FFF;
}

void sequencer_run()
{
	s16 sample = 0;

	if (apu.square1.enable_left) sample += apu.square1.output;
	if (apu.square2.enable_left) sample += apu.square2.output;
	if (apu.wave.enable_left)    sample += apu.wave.output;
	if (apu.noise.enable_left)   sample += apu.noise.output;
	sample /= 4;
	sample *= (apu.sequencer.left_volume + 1) / 8;

	// sample is between 0-15 converted to 0-255 then -128 to 127
	sample = sample * 17 - 128;
	sample = sample >> 1; // to not blast your ears

	apu.sequencer.left_sample = sample;

	sample = 0;
	if (apu.square1.enable_right) sample += apu.square1.output;
	if (apu.square2.enable_right) sample += apu.square2.output;
	if (apu.wave.enable_right)    sample += apu.wave.output;
	if (apu.noise.enable_right)   sample += apu.noise.output;
	sample /= 4;
	sample *= (apu.sequencer.right_volume + 1) / 8;

	// sample is between 0-15 converted to 0-255 then -128 to 127
	sample = sample * 17 - 128;
	sample = sample >> 1; // to not blast your ears

	apu.sequencer.right_sample = sample;
}