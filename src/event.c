#include "event.h"

bool handle_event(gameboy* gb, SDL_Event* event) 
{
	switch (event->type)
	{
		case SDL_QUIT:
			return 1;
			break;
		case SDL_KEYDOWN:
		{
			switch (event->key.keysym.sym)
			{
				case SDLK_ESCAPE:
					return 1;
					break;
			}
		}
	}

	uint8_t* state = SDL_GetKeyboardState(NULL);
	gb->b = !state[SDL_SCANCODE_Z];
	gb->a = !state[SDL_SCANCODE_X];
	gb->up = !state[SDL_SCANCODE_UP];
	gb->down = !state[SDL_SCANCODE_DOWN];
	gb->left = !state[SDL_SCANCODE_LEFT];
	gb->right = !state[SDL_SCANCODE_RIGHT];
	gb->start = !state[SDL_SCANCODE_C];
	gb->select = !state[SDL_SCANCODE_V];

	return 0;
}
