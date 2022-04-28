#include "renderer.h"
#include "imgui_renderer.h"
#include "SDL_opengl.h"
#include "ppu.h"

// 16.7427 ms / frame

static SDL_Window* debug_window;
static SDL_Window* window;
static SDL_Renderer* debug_renderer;
static SDL_Renderer* renderer;
static SDL_Event event;
static u8 scale;
static SDL_Texture* texture;
static SDL_Texture* texture2;

static u32 elapsed = 0; // ms
static u64 next_frame = 0; // ns

bool cap_framerate = false;

void renderer_init() 
{
    scale = 4;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    window = SDL_CreateWindow("BeardsBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * scale, 144 * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 160, 144);

    debug_window = SDL_CreateWindow("Debug", 0, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    debug_renderer = SDL_CreateRenderer(debug_window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);
    texture2 = SDL_CreateTexture(debug_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 160, 144);

    init_imgui(debug_window, debug_renderer, texture2);
}

void render()
{
    ppu.draw_frame = false;

    if (cap_framerate)
    {
        u64 next_frame_ms = next_frame / 1000;
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
    SDL_UpdateTexture(texture2, NULL, gb.screen_buffer, 160 * 3);
   
    render_imgui(debug_renderer);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}