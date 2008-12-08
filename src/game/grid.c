#include "grid.h"
#include "tetromino.h"

#include "../util/common.h"

#include "SDL/SDL.h"	// if i don't include this complains about memset???

// create a new grid object
Grid *grid_createNew(int w, int h) {
	Grid *grid;

	// allocate mem for new grid
	grid = malloc(sizeof(Grid));
	if (grid == NULL) { // set grid to null if we fail
		error_print("Failed to allocate memory for new grid.");
		return NULL;
	}

	// initialise grid object
	memset((void *)grid, '\0', sizeof(Grid));

	// set grid dimensions
	grid->w = w;
	grid->h = h;

	// allocate mem for grid data
	grid->data = malloc(sizeof(unsigned int) * (w * h));
	if (grid->data == NULL) {
		error_print("Failed to allocate memory for new grid data.");
		return NULL;
	}

	// initialise grid data
	grid_clear(grid);

	return grid;
}

// free grid resources
void grid_destroy(Grid *grid) {
	if (grid != NULL) {
		if (grid->data != NULL) {
			free(grid->data);
		}
		free(grid);
	}
}

// clear grid but keep same dimensions
// does nothing if NULL is supplied for a Grid
void grid_clear(Grid *grid) {
	int i;

	if (grid == NULL || grid->data == NULL) {
		return;
	}

  // set all grid positions to 0
	for (i = 0; i < (grid->w * grid->h); i++) {
		*(grid->data + i) = 0;
	}
}

// check for grid collision
// return one if a collision occurs, otherwise 0
// collisions occur when tetromino is out of bounds or in contact
// with other tetrominos
// TODO: Adjust for dynamic grid sizes
// TODO: Adjust for bottom up collision detection (better performance)
int grid_checkCollision(Grid *grid, Tetromino *tet) {
	int x, y;

	// make sure we have a tetromino and a grid to work with
	if (!grid || !tet) {
		return 0;
	}

	// collision checking
	// cycle through the tetromino line by line
	// if both the tetromino data and grid data are not 0, then we have a collision
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			if (*(tet->data + x + (y * 4)) &&
				*(grid->data + ((x + (y * GRID_WIDTH))    // add tetromino position to...
				  +	(tet->x + (tet->y * GRID_WIDTH))))) { // grid position
				debug_print("Collision with object.");
				return 1;
			}
		}
	}

	// bounds checking
	// cycle through the tetromino line by line
	// if the data has a value other than 0, check if its out of bounds
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			if (*(tet->data + x + (y * 4))) { // do i need to make this check for != 0?
				// vertical checking
				if ((y + tet->y) > (GRID_HEIGHT - 1)) { // -1 because grid height is 0 based
					debug_print("Collision with bounds.");
					return 1;
				}

				//horizontal
				if ((x + tet->x) > GRID_WIDTH - 1) {
					return 1;
				}
				else if ((x + tet->x + 1 <= 0)) {
					return 1;
				}
			}
		}
	}

  return 0;
}

// called after a collision
// will fix a tetromino in place on the grid
void grid_applyTetromino(Grid *grid, Tetromino *tet) {
  int x, y;
  int grid_pos;

  grid_pos = tet->x + (tet->y * GRID_WIDTH);
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      if (*(tet->data + x + (y * 4)) != 0) { // only apply non-blank areas of the shape
        *(grid->data + grid_pos + x + (y * GRID_WIDTH)) = *(tet->data + x + (y * 4));
      }
    }
  }
}

// checks if there are any complete lines in the grid
// if there are, 1 is returned. otherwise 0
// also an event is sent for each that is complete to trigger animation and effects
// TODO: adjust for variable grid width/height
unsigned int grid_checkLines(Grid *grid) {
  SDL_Event event;
  int x, y;
  int complete = 1;
  int completed = 0; // used for return value

  for (y = 0; y < GRID_HEIGHT; y++) {
    for (x = 0; x < GRID_WIDTH; x++) {
      if (*(grid->data + x + (y * GRID_WIDTH)) == 0) { // if x doesn't have a block...
        complete = 0;
      }
    }
    if (complete) {
      event.type = GRID_REMOVE_LINE;
      event.user.code = y;  // indicates the lines to be removed
      SDL_PushEvent(&event);
      completed = 1;
    }
    complete = 1; // set it up to scan the next row
  }

  return completed; // 1 for if there was a line completed
}

// removes the specified line from the grid, shifting all those above down a row.
void grid_removeLine(Grid *grid, int line) {
  int x, y;

  printf("Removing line: %d\n", line);

  // work from bottom to top
  for (y = line; y > 1; y--) { // 1 because you can get the line above 0 (out of bounds)
    for (x = 0; x < GRID_WIDTH; x++) {
      *(grid->data + x + (y * GRID_WIDTH)) = *(grid->data + x + ((y - 1) * GRID_WIDTH));
    }
  }
}
