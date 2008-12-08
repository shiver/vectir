#include "game.h"

#include "../util/common.h"
#include "../graphics/graphics.h"
#include "../graphics/particle.h"
#include "../graphics/font.h"
#include "../ui/ui.h"
#include "grid.h"
#include "../input/input.h"
#include "../util/config.h"

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_thread.h"

#include <stdlib.h>

#define TEXTURE_NUM 1
#define TEXTURE_GRID 0

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// global variables... yuck
static Texture *red_block_texture = NULL;
static Texture *green_block_texture = NULL;
static Texture *blue_block_texture = NULL;
static Texture *orange_block_texture = NULL;
static Texture *yellow_block_texture = NULL;
static Texture *cyan_block_texture = NULL;
static Texture *purple_block_texture = NULL;

static Texture *score_increment_texture = NULL;
static Texture *level_increment_texture = NULL;
static int previous_increment = 0;

static Texture *block_texture = NULL;
static Grid *grid = NULL;
static Tetromino *current_tetromino = NULL;
static Texture *grid_texture = NULL;
static Game game;

// need acces to the currently loaded config file
extern ConfigEntry *config;

SDL_sem *sem = NULL;

// loads all textures, sounds etc
void load_media() {
  //grid_texture = graphics_createTextureFromFile("data/grid.png");
  red_block_texture = graphics_createTextureFromFile("data/red.png");
  green_block_texture = graphics_createTextureFromFile("data/green.png");
  blue_block_texture = graphics_createTextureFromFile("data/blue.png");
  orange_block_texture = graphics_createTextureFromFile("data/orange.png");
  yellow_block_texture = graphics_createTextureFromFile("data/yellow.png");
  cyan_block_texture = graphics_createTextureFromFile("data/cyan.png");
  purple_block_texture = graphics_createTextureFromFile("data/purple.png");

	block_texture = graphics_createTextureFromFile("data/purple.png");
  font_openFont("data/Alexis.ttf", 18);

	sem = SDL_CreateSemaphore(1);
}

// triggers all the calculations
void game_update() {
	SDL_Event event;

  if (!grid || !current_tetromino) {
		return;
	}
	// check for complete lines
  grid_checkLines(grid);

  // move tetromino down and check for collisions
  tetromino_moveDown(current_tetromino);
  if (grid_checkCollision(grid, current_tetromino) == 1) {
    tetromino_moveUp(current_tetromino);
    grid_applyTetromino(grid, current_tetromino);

		// update the grid prior to new tet being created
		grid_checkLines(grid);

    // todo: consider handling this in the GRID_REMOVE_LINE event
    tetromino_destroy(current_tetromino);
		current_tetromino = NULL;

		event.type = TETROMINO_CREATE;
		SDL_PushEvent(&event);
  }
}

Uint32 game_updateTime(Uint32 interval, void *param) {
  if (game.seconds < 59) {
    game.seconds++;
    return interval;
  }
  else {
    game.seconds = 0;

    if (game.minutes < 59) {
      game.minutes++;
      return interval;
    }
    else {
      game.minutes = 0;

      if (game.hours < 23) { // if you've been playing this long... i feel sorry for you
        game.hours++;
        return interval;
      }
      else { // now i feel VERY sorry for you
        game.hours = 0; // so ill make it look like you haven't done anything
        // perhaps its time to go outside :D
		return interval;
      }
    }
  }
	return interval; // dont expect to get here...
}

// return the game running time in hh:mm:ss format
void game_getTime(char time_buffer[]) {
  sprintf(time_buffer, "%02d:%02d:%02d", game.hours, game.minutes, game.seconds);
}

// returns the frequency in milliseconds that we need to recalculate game state
unsigned int game_getGameUpdateFreq() {
  int i;

  i = GAME_UPDATE_FREQUENCY - (game.level * 25);
  if (i <= 0) {
    return 5;
  }
  else {
    return i;
  }
}

void game_setStartingLevel(int level) {
}

int game_getStartingLevel() {
	return 0;
}

// render stuff
void display_update() {
	int gx, gy;
	char buffer[20];
	char time_buffer[8];
	Particle *current_particle = NULL;
	ParticleCollection *current_collection = NULL;
	SDL_Color grid_color;

	// TODO: make grid color settable
	grid_color.r = 255;
	grid_color.g = 255;
	grid_color.b = 255;

	graphics_clear();

  // draw texture for the grid
	if (grid) {
		graphics_drawGrid(20, 0, GRID_BLOCK_SIZE, GRID_WIDTH, GRID_HEIGHT, &grid_color);	
	}

  // draw grid
  if (grid && block_texture) {
		for (gy = 0; gy < GRID_HEIGHT; gy++) { // TODO: cater for realtime grid width/height settings
			for (gx = 0; gx < GRID_WIDTH; gx++) {
				// red
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 1) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, red_block_texture);
				}
				// green
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 2) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, green_block_texture);
				}
				// blue
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 3) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, blue_block_texture);
				}
				// orange
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 4) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, orange_block_texture);
				}
				// yellow
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 5) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, yellow_block_texture);
				}
				// cyan
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 6) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, cyan_block_texture);
				}
				// purple
				if (*(grid->data + gx + (gy * GRID_WIDTH)) == 7) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx),
          0 + (GRID_BLOCK_SIZE * gy), GRID_BLOCK_SIZE,
          GRID_BLOCK_SIZE, purple_block_texture);
				}
			}
		}
  }

  // draw tetromino
  if (block_texture && current_tetromino) {
    for (gy = 0; gy < 4; gy++) { // TODO: cater for realtime grid width/height settings
			for (gx = 0; gx < 4; gx++) {
			  // red
				if (*(current_tetromino->data+gx+(4*gy)) == 1) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, red_block_texture);
				}
				// green
				if (*(current_tetromino->data+gx+(4*gy)) == 2) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, green_block_texture);
				}
				// blue
				if (*(current_tetromino->data+gx+(4*gy)) == 3) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, blue_block_texture);
				}
				// orange
				if (*(current_tetromino->data+gx+(4*gy)) == 4) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, orange_block_texture);
				}
				// yellow
				if (*(current_tetromino->data+gx+(4*gy)) == 5) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, yellow_block_texture);
				}
				// cyan
				if (*(current_tetromino->data+gx+(4*gy)) == 6) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, cyan_block_texture);
				}
				// purple
				if (*(current_tetromino->data+gx+(4*gy)) == 7) {
					graphics_drawTextureScaled(20 + (GRID_BLOCK_SIZE * gx) +
          (GRID_BLOCK_SIZE * current_tetromino->x),
            0 + (GRID_BLOCK_SIZE * gy) + (GRID_BLOCK_SIZE *
            current_tetromino->y), GRID_BLOCK_SIZE, GRID_BLOCK_SIZE, purple_block_texture);
				}
			}
		}
  } 

  // draw text
  font_print(280, 35, "SCORE", NULL);
	sprintf(buffer, "%d", game.score);
  font_print(280, 55, buffer, NULL);
  font_print(280, 75, "TIME", NULL);
  game_getTime(time_buffer);
  font_print(280, 95, time_buffer, NULL);
  font_print(280, 115, "LEVEL", NULL);
	sprintf(buffer, "%d", game.level);
  font_print(280, 135, buffer, NULL);
  font_print(280, 155, "LINES", NULL);
	sprintf(buffer, "%d", game.lines);
  font_print(280, 175, buffer, NULL); 

  // render any particles if they're not currently being modified
	if (SDL_SemTryWait(sem) == 0) {
		current_collection = particle_getActiveCollections();
		if (current_collection) {
 	  	while (current_collection) {
      	current_particle = current_collection->first;
	
      	while (current_particle) {
      		glColor4f(1.0f, 1.0f, 1.0f, current_particle->alpha);
    	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  	      glEnable(GL_BLEND);
	
  	      graphics_drawTexture(current_particle->x, current_particle->y, current_particle->texture);
	        glDisable(GL_BLEND);

        	current_particle = current_particle->next;
      	}

      	current_collection = current_collection->next;
    	}
  	}
		SDL_SemPost(sem);
	}

  // draw the menu if its needed
	if (ui_isMenuVisible()) {
		if (grid && current_tetromino) {
			ui_drawMainMenu(MENU_IN_GAME);
		}
		else {
			ui_drawMainMenu(MENU_MAIN);
		}
	}

	SDL_GL_SwapBuffers();
}

// creates some particles to show score incrememnts
void game_showScoreIncrement(int line, int increment) {
	int i;
	char buffer[5];
	Particle *first = NULL;
	Particle *current = NULL;

	for (i = 0; i < GRID_WIDTH; i++) {
		if (first == NULL) { // if we dont have any particles yet
      first = particle_createNewParticle();
      current = first;

      graphics_clear(); // clear the back buffer

      // create the texture for the increments
      // TODO: adjust for adjustable resolution
      if (!score_increment_texture) {
				sprintf(buffer, "+%d", increment);
				font_print(0, 0, buffer, NULL); // put the text into the back buffer
		    glFlush();				
				score_increment_texture = graphics_createTextureFromViewport(0, 
																																		 SCREEN_HEIGHT - 128, 
																																		 128, 128);
			}
			else {
				if (increment != previous_increment) {
					sprintf(buffer, "+%d", increment);
					font_print(0, 0, buffer, NULL); // put the text into the back buffer
		      glFlush();	
					graphics_adjustTextureFromViewport(0, SCREEN_HEIGHT - 128, 128, 128, 
																						score_increment_texture);
				}
			}
			current->texture = score_increment_texture;
    }
    else { // if we're adding to already existing
      current->next = particle_createNewParticle();
      current = current->next;
      current->texture = first->texture;
    }

		// set some initial particle states
		current->alpha = 0.8f;
		current->alpha_decay = -((_random(2) + 1) * 0.01);
		current->v_speed = -((_random(10) + 1) * 0.1);
    current->x = 20 + (GRID_BLOCK_SIZE * i);
	    current->y = (GRID_BLOCK_SIZE/2) + (GRID_BLOCK_SIZE * line);
	}

  // add to the collection
  particle_createNewCollection(first);

	previous_increment = increment;
}

void game_showLevelIncrement() {
	char *buffer[9]; // level xx\0
	Particle *particle = NULL;

	particle = particle_createNewParticle();
	graphics_clear();
	sprintf((void*)buffer, "LEVEL %d", game.level);
	font_print(0, 0, (void*)buffer, NULL);
	glFlush();

	if (!level_increment_texture) {
		level_increment_texture = graphics_createTextureFromViewport(0, SCREEN_HEIGHT - 128,
																															128, 128);
	} else {
		graphics_adjustTextureFromViewport(0, SCREEN_HEIGHT - 128, 128, 128,
			level_increment_texture);
	}
	
	particle->texture = level_increment_texture;
	particle->alpha = 1.0f;
	particle->alpha_decay = -0.02f;
	particle->v_speed = 1.0f;
	particle->x = GRID_BLOCK_SIZE + ((GRID_BLOCK_SIZE * GRID_WIDTH) / 2) - 32;
	particle->y = GRID_BLOCK_SIZE + ((GRID_BLOCK_SIZE * GRID_HEIGHT) / 2) - 64;
	
	particle_createNewCollection(particle);

}

// the main game loop which will run until the game is done
void game_loop() {
	SDL_Event event, e;
	SDL_TimerID timer;
	int last_game_update = 0;
	int last_particle_update = 0;
	int last_render = 0;
	int previous_level = 0;

	debug_print("Loading media...");
	load_media();

	// check if we want to show score increments
	game.show_score_increments = atoi(config_getValue("show_score_increments"));

	// setup input
	input_init();

	// create & show menu
	ui_menuInit();
	ui_toggleMenuVisible();
	SDL_SetEventFilter(ui_handleEvents);

	// loop forever
	debug_print("Entering main game loop...");
	while (1) {
		// see if its time to trigger an update (make sure we're not paused either)
		if (!game.paused && 
					SDL_GetTicks() - last_game_update > game_getGameUpdateFreq()) {
			last_game_update = SDL_GetTicks();		// remember time of last update
			game_update();
		}

		if (game.show_score_increments) { // at the moment we just have the one particle set
			// see if its time to trigger a particle update
			if (SDL_GetTicks() - last_particle_update > PARTICLE_UPDATE_INTERVAL) {
				last_particle_update = SDL_GetTicks();
				particle_update();
			}
		}

		// check for any events waiting
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				// key presses are handled in input.c
				case SDL_KEYDOWN:
					input_onKeyDown(event.key.keysym.sym);
					break;
				case LEFT_KEY:
					tetromino_moveLeft(current_tetromino);
					if (grid_checkCollision(grid, current_tetromino)) {
						tetromino_moveRight(current_tetromino);
					}
					break;
        case RIGHT_KEY:
          tetromino_moveRight(current_tetromino);
          if (grid_checkCollision(grid, current_tetromino)) {
            tetromino_moveLeft(current_tetromino);
          }
          break;
        case DOWN_KEY:
					// uses the key repeat interval to accelerate the tetromino down
          tetromino_moveDown(current_tetromino);
          if (grid_checkCollision(grid, current_tetromino)) {
            tetromino_moveUp(current_tetromino);
					}
					break;
        case UP_KEY:
          // rotate to a new position
          tetromino_setNextPosition(current_tetromino);
          tetromino_setShape(current_tetromino, current_tetromino->type,
            current_tetromino->position);

          // make sure the new position doesn't cause any collisions
          // if it does, reverse the rotation
          if (grid_checkCollision(grid, current_tetromino)) {
            tetromino_setPrevPosition(current_tetromino);
            tetromino_setShape(current_tetromino, current_tetromino->type,
            current_tetromino->position);
          }
          break;
				case SPACE_KEY:
					tetromino_moveDown(current_tetromino);

					// move the tetromino down until it causes a collision
					while (!grid_checkCollision(grid, current_tetromino)) {
						tetromino_moveDown(current_tetromino);
					}
					// once we have a collision, move it back into place
					tetromino_moveUp(current_tetromino);
					break;
				case PAUSE_KEY:
					debug_print("Pausing game");
					game.paused = !game.paused;

					// stop the game timer
					if (game.paused) {
						SDL_RemoveTimer(timer);
						timer = NULL;
					}
					else { // start it again
						timer = SDL_AddTimer(1000, game_updateTime, NULL);
					}
					break;
				case ESCAPE_KEY:
					// pause game updates
					debug_print("Escape key pressed.");
					
					// toggle paused game state if we're in game
					if (grid && current_tetromino) {
						game.paused = !game.paused;
						if (game.paused) { // stop couting time played
							SDL_RemoveTimer(timer);
							timer = NULL;
						}
						// starting timer again, only if we're still in a game
						else if (grid && current_tetromino) { 
							timer = SDL_AddTimer(1000, game_updateTime, NULL);
						}
					}

					// show or hide the menu
					if (grid && current_tetromino) {
						ui_toggleMenuVisible();
					}
					
					// enable ui message pump if its visible
					if (ui_isMenuVisible()) {
						// if we're in game, show in-game menu
						if (grid && current_tetromino) { 
							ui_menuPageSetCurrentById(MENU_IN_GAME);
						}	
						// otherwise show main menu
						else {
							ui_menuPageSetCurrentById(MENU_MAIN);
						}
						SDL_SetEventFilter(ui_handleEvents);
					}
					break;
				case GAME_START_NEW:
					// set some game variables
					game.level = 0;
					game.score = 0;
					game.lines = 0;
					game.paused = 0;

				  // time variables
					game.hours = 0;
					game.minutes = 0;
					game.seconds = 0;

					// create the grid
					grid = grid_createNew(GRID_WIDTH, GRID_HEIGHT);

				  // create the first tetromino
				  current_tetromino = tetromino_createNew();
				  current_tetromino->x = 0;
  				current_tetromino->y = 0;
  				
					// update time
				  SDL_Init(SDL_INIT_TIMER);
				  
					if (timer) {
						SDL_RemoveTimer(timer);
						timer = NULL;
					}

					ui_toggleMenuVisible();
					timer = SDL_AddTimer(1000, game_updateTime, NULL);
					game.paused = 0;
					break;
				case GAME_END: // called by either the menu or game over scenario
					// destroy timer, grid and tetromino
					SDL_RemoveTimer(timer);
					timer = NULL;

					grid_destroy(grid);
					grid = NULL;
					tetromino_destroy(current_tetromino);
					current_tetromino = NULL;

					// show menu if it isn't already visible
					ui_menuPageSetCurrentById(MENU_MAIN);
					if (!ui_isMenuVisible()) {
						SDL_SetEventFilter(ui_handleEvents);
						ui_toggleMenuVisible();
					}
					break;
				case TETROMINO_CREATE:
					// assumes that the old one has already been discarded
					current_tetromino = tetromino_createNew();
			    current_tetromino->x = 0;
			    current_tetromino->y = 0;

					// check if we have an immediate collision (game over)
					if (grid_checkCollision(grid, current_tetromino)) {
						SDL_RemoveTimer(timer);
						timer = NULL;
						
						e.type = GAME_END;
						SDL_PushEvent(&e);
					}
					break;
        case GRID_REMOVE_LINE:
					if (game.show_score_increments) {
          	// animated score increment
						game_showScoreIncrement(event.user.code, (game.level + 1) * 10);
					}

          grid_removeLine(grid, event.user.code);

          // increment number of complete lines
          game.lines += 1;
          // +10 per block and x10 per level
          game.score += (game.level + 1) * 10 * GRID_WIDTH;

          // increment the game level every 10 lines
          previous_level = game.level;
					game.level = game.lines / 10;
					if (previous_level != game.level) {
						game_showLevelIncrement();
					}

          break;
        case GAME_QUIT:
          SDL_RemoveTimer(timer); // stop gameplay timer
          timer = NULL;
					game_shutdown();
            break;
        // unhandled events are ignored
  			default:
  				break;
  		}
    }

		// update the display
		// without this delay gfx card tries to commit suicide by melting
		if (SDL_GetTicks() - last_render > 3) {
			display_update();
			last_render= SDL_GetTicks();
		}
	}
}

// deal with all the things that need to be shut down
void game_shutdown() {
  debug_print("Freeing texture resources.");
	graphics_destroyTexture(block_texture);
  graphics_destroyTexture(grid_texture);
	debug_print("Game shutting down...");

  font_destroy();
	graphics_shutdown();

	tetromino_destroy(current_tetromino);
	grid_destroy(grid);

	exit(1);
}
