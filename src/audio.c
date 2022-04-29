#include "audio.h"
#include "apu.h"
#include "SDL.h"
#include "SDL_audio.h"

static SDL_AudioSpec audio_spec;
SDL_AudioSpec request;
SDL_AudioDeviceID audio_device;

//SAMPLE_RATE 48000;
//BUFFER_SIZE 1024;


void audio_init()
{
    SDL_memset(&audio_spec, 0, sizeof(audio_spec));
    audio_spec.freq = 4800;
    audio_spec.format = AUDIO_S16LSB;
    audio_spec.channels = 2;
    audio_spec.samples = 4096;
    audio_spec.silence = 0;
    audio_spec.callback = audio_callback;
    audio_spec.userdata = &apu;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, 0, SDL_AUDIO_ALLOW_ANY_CHANGE);

    SDL_PauseAudio(0);
    //SDL_Delay(5000);
    SDL_CloseAudio();
    //SDL_QueueAudio(1, data, len);
}

static Uint8* audio_chunk;
static Uint32 audio_len;
static Uint8* audio_pos;


void audio_callback(void* userdata, u8* stream, int len)
{
    //float* out = (float*)stream;
    //for (int i = 0; i < len / sizeof(float); i++)
    //{
    //    if (apu.buffer_size.read_index < apu.buffer_size.write_index)
    //    {
    //        apu.apu_last_sample = apu.bigbuffer.buf[(apu->bigbuffer.read_index++) % AUDIO_BIGBUFFER_SIZE];
    //    }
    //}

    //for (int i = 0; i < len; i++)
    //    stream[i] = i;

    //*out++ = apu->apu_last_sample
}

