#pragma once
#include "types.h"

void audio_init();
void audio_callback(void* userdata, u8* stream, int len);
