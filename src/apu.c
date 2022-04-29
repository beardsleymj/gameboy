#include "apu.h"
#include "gb_system.h"
#include <string.h>

apu_t apu;

apu_run()
{
	// run square1
	// run square 2
	// run wave
	// run noise
	// run freq sequencer


	// Tick Frequenvy Timers
	//update_freq_timer(&apu.ch1, apu.nr1314.frequency);
	update_freq_timer(&apu.ch2, apu.nr2324.frequency);
	//update_freq_timer(&apu.ch3, apu.nr3334.frequency);


	// Update Frame Sequencer
	if (gb.cycles - apu.frame_sequencer_last_cycle >= 8192)
	{	
		apu.frame_sequencer = (apu.frame_sequencer + 1) & 0x7;
		apu.frame_sequencer_last_cycle += 8192;
	}
	else
	{
		return;
	}

	// tick functions based on frame sequencer
	switch (apu.frame_sequencer)
	{
		case 0:
/*			update_envelope*/
			//step_lengths
			break;

		case 1:
			break;

		case 2:
			/*step_lengths*/
			/*step_sweep*/
			break;

		case 3:
			break;

		case 4:
			/*step_length*/
			break;

		case 5:
			break;

		case 6:
			/*step_length*/
			/*step_sweep*/
			break;

		case 7:
			update_volume(&apu.ch1, apu.nr12);
			update_volume(&apu.ch2, apu.nr22);
			update_volume(&apu.ch4, apu.nr42);
			break;
	}

	if (apu.nr52.s1_on)
	{
		u16 ch1_freq = get_ch1_freq();
	}

	if (apu.nr52.s2_on)
	{
		u16 ch2_freq = get_ch2_freq();
	}
	
	if (apu.nr52.s3_on)
	{
		u16 ch3_freq = get_ch3_freq();
	}

	if (apu.nr52.s4_on)
	{
		u16 ch4_freq = get_ch4_freq();
	}
}

u16 get_ch1_freq()
{

}

// There are 4 waveform pattern duties 
const u8 wave_pattern_duties[4][8] =
{
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 0
};

u16 get_ch2_freq()
{
	u8 amplitude = wave_pattern_duties[apu.nr21.wave_pattern_duty][apu.ch2.wave_position];
}

void update_freq_timer(channel_t* channel, u16 frequency)
{
	while (gb.cycles - channel->last_freq_timer_cycle >= channel->frequency_timer)
	{
		channel->last_freq_timer_cycle++;
		channel->frequency_timer--;

		if (channel->frequency_timer == 0)
		{
			channel->frequency_timer = (2048 - frequency) * 4;
			channel->wave_position = (channel->wave_position + 1) & 7;
		}
	}
}

u16 get_ch3_freq()
{

}

u16 get_ch4_freq()
{

}

// need to figure out what period_timer is 
update_volume(channel_t* ch, volume_envelope_t ve)
{
	if (ve.period == 0)
	{
		return;
	}
	
	if (ch->envelope_timer > 0)
	{
		ch->envelope_timer--;
		return;
	}

	if (ch->envelope_timer == 0)
	{
		ch->envelope_timer = ve.period;

		if (ve.volume < 0xF && ve.direction)
		{
			ve.period++;
		}
			
		if (ve.period > 0x0 && !ve.direction)
		{
			ve.period--;
		}
	}
}