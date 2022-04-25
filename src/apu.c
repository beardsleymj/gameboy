#include "apu.h"
#include "SDL.h"
#include "SDL_audio.h"
#include "types.h"
#include <stdio.h>

void test_audio()
{
    SDL_AudioSpec audio_settings;

    SDL_memset(&audio_settings, 0, sizeof(audio_settings));
    audio_settings.freq = 4800;
    audio_settings.format = AUDIO_S16LSB;
    audio_settings.channels = 2;
    audio_settings.silence = 0;
    audio_settings.samples = 4096;
    audio_settings.callback = SDLAudioCallback;

    SDL_OpenAudioDevice(NULL, 0, &audio_settings, 0, SDL_AUDIO_ALLOW_ANY_CHANGE);

    SDL_PauseAudio(0);
    //SDL_Delay(5000);
    SDL_CloseAudio();
}

void SDLAudioCallback(void* userdata, u8* stream, s64 len)
{   // clear buffer to silence
    memset(stream, 0, len);
}