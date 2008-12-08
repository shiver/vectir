#ifndef GRID_H
#define GRID_H

#include "tetromino.h"

// GRID settings
#define GRID_WIDTH  10
#define GRID_HEIGHT 18
#define GRID_BLOCK_SIZE 20

// GRID events
#define GRID_REMOVE_LINE SDL_USEREVENT+5

typedef struct _grid {
	int w, h;	// width, height
	unsigned int* data;
} Grid;

Grid *grid_createNew(int, int);
void grid_destroy(Grid*);
void grid_clear(Grid*);
int grid_checkCollision(Grid*, Tetromino*);
void grid_applyTetromino(Grid*, Tetromino*);
unsigned int grid_checkLines(Grid*);
void grid_removeLine(Grid*, int);

#endif

