#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gb_system.h"
#include "SDL.h"

void renderer_init();
void render();

extern bool cap_framerate;

#ifdef __cplusplus
}
#endif