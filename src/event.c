#include "gb_system.h"
#include "event.h"
#include "renderer.h"
#include "imgui_renderer.h"

bool handle_events() 
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		process_imgui_event(&event);
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{	// closing both windows for now, maybe do one later
					case SDL_WINDOWEVENT_CLOSE:
						return 1;
						//SDL_DestroyWindow();
						break;
				}
				break;

			case SDL_QUIT:
				return 1;
				break;

			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return 1;
						break;

					case SDLK_1 :
						cap_framerate = !cap_framerate;
						break;
				}
			}
		}
	}
	return 0;
}

void update_gb_keystate()
{
	u8* state = SDL_GetKeyboardState(NULL);
	gb.b = !state[SDL_SCANCODE_Z];
	gb.a = !state[SDL_SCANCODE_X];
	gb.up = !state[SDL_SCANCODE_UP];
	gb.down = !state[SDL_SCANCODE_DOWN];
	gb.left = !state[SDL_SCANCODE_LEFT];
	gb.right = !state[SDL_SCANCODE_RIGHT];
	gb.start = !state[SDL_SCANCODE_C];
	gb.select = !state[SDL_SCANCODE_V];
}