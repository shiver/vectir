#include "input.h"

#include "../util/common.h"

// TODO: handle custom key configurations
void input_onKeyDown(SDLKey key) {
  SDL_Event event;
  
  switch(key) {
    case SDLK_LEFT:
      event.type = LEFT_KEY;
      SDL_PushEvent(&event);
      break;
    case SDLK_RIGHT:
      event.type = RIGHT_KEY;
      SDL_PushEvent(&event);
      break;
    case SDLK_DOWN:
      event.type = DOWN_KEY;
      SDL_PushEvent(&event);
      break;
    case SDLK_UP:
      event.type = UP_KEY;
      SDL_PushEvent(&event);
      break;
    case SDLK_q:
      event.type = GAME_QUIT;
      SDL_PushEvent(&event);
      break;
		case SDLK_SPACE:
			event.type = SPACE_KEY;
			SDL_PushEvent(&event);
			break;
		case SDLK_PAUSE:
			event.type = PAUSE_KEY;
			SDL_PushEvent(&event);
			break;
		case SDLK_ESCAPE:
			event.type = ESCAPE_KEY;
			SDL_PushEvent(&event);
			break;
    default:
      break;
  }
}

void input_init() {
  debug_print("Initialising input handling.");
  SDL_EnableKeyRepeat(300, 50);
}
