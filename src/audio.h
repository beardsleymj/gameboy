#pragma once
#include "types.h"

#define SAMPLE_RATE 48000
#define SAMPLES 2048

void audio_init();
void audio_queue_sample(s8* data);
void audio_clear();