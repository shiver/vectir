#include "util/common.h"
#include "graphics/graphics.h"
#include "graphics/font.h"
#include "game/game.h"
#include "util/config.h"

#include <stdio.h>
#include <string.h>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

int main(int argc, char* argv[]) {
	char *token;

	debug_print("Starting...");
  config_load("config.txt");
	
	// set some initial values from config file
	token = strtok(config_getValue("resolution"), "x");
	SCREEN_WIDTH = atoi(token);
	token = strtok(NULL, "x");
	SCREEN_HEIGHT = atoi(token);

	graphics_init();
  font_init();

	game_loop();

	return 0;
}

