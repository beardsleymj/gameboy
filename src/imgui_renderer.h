#pragma once
#include "SDL.h"

#ifdef __cplusplus
extern "C" 
{
#endif

void process_imgui_event(SDL_Event* event);
void init_imgui(SDL_Window* window, SDL_Renderer* gl_context, SDL_Texture* tex);
void render_imgui(SDL_Renderer* debug_renderer);

#ifdef __cplusplus
}
#endif