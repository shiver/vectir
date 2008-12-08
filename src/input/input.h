#ifndef INPUT_H
#define INPUT_H

#include "SDL/SDL.h"

// define custom events for input handling
#define LEFT_KEY   SDL_USEREVENT
#define RIGHT_KEY  SDL_USEREVENT+1
#define DOWN_KEY   SDL_USEREVENT+2
#define UP_KEY     SDL_USEREVENT+3
#define SPACE_KEY  SDL_USEREVENT+7
#define PAUSE_KEY  SDL_USEREVENT+8
#define ESCAPE_KEY SDL_USEREVENT+9
#define GAME_QUIT  SDL_USEREVENT+4

void input_init();
void input_onKeyDown(SDLKey);

#endif
