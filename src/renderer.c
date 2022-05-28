#include "renderer.h"
#include "imgui_renderer.h"
#include "ppu.h"
#include "SDL.h"
#include "glad/glad.h"
#include "SDL_opengl.h"

// 16.7427 ms / frame

static SDL_Window* window;
static SDL_GLContext context;
static SDL_Event event;
static u8 scale;
static GLuint texture;
static u32 elapsed = 0; // ms
static u64 next_frame = 0; // ns

bool cap_framerate = true;



void renderer_init() 
{
    scale = 4;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Game Boy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * scale, 144 * scale, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(0);
	
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, gb.screen_buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
    init_imgui(window, context, texture);
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

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144,GL_RGB, GL_UNSIGNED_BYTE, gb.screen_buffer);
   
    render_imgui();
}