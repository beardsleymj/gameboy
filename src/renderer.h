#pragma once

#include "gb_system.h"

#ifdef __cplusplus
extern "C" {
#endif

void renderer_init();
void render();

extern bool cap_framerate;

#ifdef __cplusplus
}
#endif