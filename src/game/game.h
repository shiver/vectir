#ifndef GAME_H
#define GAME_H

#include "SDL/SDL.h"

// the starting game update frequency
#define GAME_UPDATE_FREQUENCY	900

#define GAME_START_NEW			SDL_USEREVENT+10
#define GAME_END				SDL_USEREVENT+11

typedef struct _game {
  unsigned int level;
  unsigned int score;
  unsigned int lines;

	int paused;
	int show_score_increments;

  // time
  char hours;
  char minutes;
  char seconds;
} Game;

void game_loop();
void game_shutdown();
void game_getLevel();
unsigned int game_getLevelUpdateFreq();
void game_getScore();
void game_getTime(char[]);
void game_getLines();
Uint32 game_updateTime(Uint32, void*);
void game_showScoreIncrement(int, int);

#endif

