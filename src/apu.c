#include "apu.h"
#include "SDL.h"
#include "SDL_audio.h"
#include "types.h"
#include <stdio.h>

static u32 SamplesPerSecond = 48000;
u32 ToneHz = 256;
s16 ToneVolume = 3000;
u32 RunningSampleIndex = 0;
s32 SquareWavePeriod = 48000 / 256;
s32 BytesPerSample = sizeof(s16) * 2;




void test_audio()
{
    SDL_CloseAudio();
}