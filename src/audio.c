#include "audio.h"
#include "apu.h"
#include "SDL.h"
#include "SDL_audio.h"

static SDL_AudioSpec audio_spec;
SDL_AudioSpec request;
SDL_AudioDeviceID audio_device;

void audio_init()
{
    if (SDL_Init(SDL_INIT_AUDIO))
        printf("SDL Failed to Initialize Audio");

    SDL_memset(&audio_spec, 0, sizeof(audio_spec));
    audio_spec.freq = SAMPLE_RATE;
    audio_spec.format = AUDIO_S8;
    audio_spec.channels = 2; // THERE IS ONLY 1 CHANNEL RIGHT NOW
    audio_spec.samples = SAMPLES;
    audio_spec.silence = 0;
    //audio_spec.callback = audio_callback;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, 0, 0);
    const char* device = SDL_GetAudioDeviceName(audio_device, 0);
    if (device != NULL)
        printf(device);
	
    SDL_PauseAudioDevice(audio_device, 0);
}

void audio_queue_sample(s8 *data)
{	
    int queue_size = SDL_GetQueuedAudioSize(audio_device);
    if (queue_size > SAMPLE_BUFFER_SIZE)
    {
        //printf("%i size ", queue_size);
        return;
    }

    apu.samples.buffer[apu.samples.write_index++] = *data;
    if (apu.samples.write_index == SAMPLE_BUFFER_SIZE)
    {
        apu.samples.write_index = 0;
        
        SDL_QueueAudio(audio_device, apu.samples.buffer, sizeof(apu.samples.buffer));
    }
}

void audio_clear()
{
    SDL_ClearQueuedAudio(audio_device);
}