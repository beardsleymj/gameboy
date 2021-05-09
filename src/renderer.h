#pragma once
#include <SDL.h>
#include <gl\glew.h>
#include <GL\GLU.h>
#include <SDL_opengl.h>
#include "gb.h"

void renderer_init();
void render(gameboy* gb);
void fill_image(gameboy*);
