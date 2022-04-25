#include "renderer.h"

// 16.7427 ms / frame

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Event event;
static u8 scale;
static SDL_Texture* texture;

static u32 elapsed = 0; // ms
static u64 next_frame = 0; // ns

bool cap_framerate = false;

void renderer_init() 
{
    scale = 4;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);;
    window = SDL_CreateWindow("BeardsBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * scale, 144 * scale, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, 160, 144);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}

void render()
{
    gb.draw_frame = false;

    // TODO: this falls behind because elapsed > next_frame_ms quite often,
    // will probably try to sync to audio later
    if (cap_framerate)
    {
        u32 next_frame_ms = next_frame / 1000;
        elapsed = SDL_GetTicks();
        if (next_frame_ms >= elapsed)
        {
            SDL_Delay(next_frame_ms - elapsed);
            next_frame += 16742;
        }
        else
        {
            next_frame = (elapsed * 1000) + 16742;
        }
    }

    SDL_UpdateTexture(texture, NULL, gb.screen_buffer, 160 * 3);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(renderer);


}