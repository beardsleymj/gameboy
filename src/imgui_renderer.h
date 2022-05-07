#pragma once
#include "SDL.h"
#include "glad/glad.h"

#ifdef __cplusplus
extern "C" 
{
#endif

void process_imgui_event(SDL_Event* event);
void init_imgui(SDL_Window* win, SDL_GLContext gl_context, GLuint gl_texture);
void render_imgui();
void draw_menu_bar();
void draw_system();
void draw_debugger();
void draw_memory_editor();

#ifdef __cplusplus
}
#endif