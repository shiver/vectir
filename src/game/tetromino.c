#include "tetromino.h"

#include "../util/common.h"

#include "SDL/SDL.h"

// I
char I_SHAPE_POS_A[16] =
   {1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};
char I_SHAPE_POS_B[16] =
   {0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0};

// L
char L_SHAPE_POS_A[16] =
   {0, 0, 2, 0,
    2, 2, 2, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

char L_SHAPE_POS_B[16] =
   {0, 2, 0, 0,
    0, 2, 0, 0,
    0, 2, 2, 0,
    0, 0, 0, 0};

char L_SHAPE_POS_C[16] =
   {0, 0, 0, 0,
    2, 2, 2, 0,
    2, 0, 0, 0,
    0, 0, 0, 0};

char L_SHAPE_POS_D[16] =
   {2, 2, 0, 0,
    0, 2, 0, 0,
    0, 2, 0, 0,
    0, 0, 0, 0};

// J
char J_SHAPE_POS_A[16] =
   {3, 0, 0, 0,
    3, 3, 3, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

char J_SHAPE_POS_B[16] =
   {0, 3, 3, 0,
    0, 3, 0, 0,
    0, 3, 0, 0,
    0, 0, 0, 0};

char J_SHAPE_POS_C[16] =
   {0, 0, 0, 0,
    3, 3, 3, 0,
    0, 0, 3, 0,
    0, 0, 0, 0};

char J_SHAPE_POS_D[16] =
   {0, 3, 0, 0,
    0, 3, 0, 0,
    3, 3, 0, 0,
    0, 0, 0, 0};

// O
char O_SHAPE_POS_A[16] =
   {0, 0, 0, 0,
    0, 4, 4, 0,
    0, 4, 4, 0,
    0, 0, 0, 0};

// S
char S_SHAPE_POS_A[16] =
   {0, 5, 5, 0,
    5, 5, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

char S_SHAPE_POS_B[16] =
   {0, 5, 0, 0,
    0, 5, 5, 0,
    0, 0, 5, 0,
    0, 0, 0, 0};

// Z
char Z_SHAPE_POS_A[16] =
   {6, 6, 0, 0,
    0, 6, 6, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

char Z_SHAPE_POS_B[16] =
   {0, 6, 0, 0,
    6, 6, 0, 0,
    6, 0, 0, 0,
    0, 0, 0, 0};

// T
char T_SHAPE_POS_A[16] =
   {0, 7, 0, 0,
    7, 7, 7, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

char T_SHAPE_POS_B[16] =
   {0, 7, 0, 0,
    0, 7, 7, 0,
    0, 7, 0, 0,
    0, 0, 0, 0};

char T_SHAPE_POS_C[16] =
   {0, 0, 0, 0,
    7, 7, 7, 0,
    0, 7, 0, 0,
    0, 0, 0, 0};

char T_SHAPE_POS_D[16] =
   {0, 7, 0, 0,
    7, 7, 0, 0,
    0, 7, 0, 0,
    0, 0, 0, 0};



// creates a random new tetromino
Tetromino *tetromino_createNew() {
  Tetromino *tet;

	tet = malloc(sizeof(Tetromino));
	if (tet == NULL) {
		error_print("Failed to allocate memory for new tetromino.");
		return NULL;
	}

  // initialise the tetromino object
	memset((void *)tet, '\0', sizeof(Tetromino));

  // allocate mem for tetromino data
  tet->data = malloc(sizeof(unsigned int) * 16);
  if (tet->data == NULL) {
		error_print("Failed to allocate memory for new tetromino data.");
		return NULL;
	}

  tetromino_setShape(tet, _random(7), -1);  // use -1 here for random position
  return tet;
}

// frees resources used by the supplied tetromino
void tetromino_destroy(Tetromino *tet) {
  if (tet != NULL) {
    if (tet->data != NULL) {
      free(tet->data);
    }
    free(tet);
  }
}

// applies the type and position specified to the supplied tetromino
// -1 for position will randomise
void tetromino_setShape(Tetromino *tet, int type, int position) {
  int i;

  tet->type = type;
  tet->position = position;

  switch(type) {
    case J_SHAPE:
      tet->max_positions = 4;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case J_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = J_SHAPE_POS_A[i];
          }
          break;
        case J_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = J_SHAPE_POS_B[i];
          }
          break;
        case J_SHAPE_POS_3:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = J_SHAPE_POS_C[i];
          }
          break;
        case J_SHAPE_POS_4:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = J_SHAPE_POS_D[i];
          }
          break;
      }
      break;
    case L_SHAPE:
      tet->max_positions = 4;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case L_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = L_SHAPE_POS_A[i];
          }
          break;
        case L_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = L_SHAPE_POS_B[i];
          }
          break;
        case L_SHAPE_POS_3:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = L_SHAPE_POS_C[i];
          }
          break;
        case L_SHAPE_POS_4:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = L_SHAPE_POS_D[i];
          }
          break;
      }
      break;
    case I_SHAPE:
      tet->max_positions = 2;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case I_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = I_SHAPE_POS_A[i];
          }
          break;
        case I_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = I_SHAPE_POS_B[i];
          }
          break;
      }
      break;
    case S_SHAPE:
      tet->max_positions = 2;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case S_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = S_SHAPE_POS_A[i];
          }
          break;
        case S_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = S_SHAPE_POS_B[i];
          }
          break;
      }
      break;
    case O_SHAPE:
      tet->max_positions = 1;
      // position is ignored since only one is possible
      for(i = 0; i < 16; i++) {
        *(tet->data+i) = O_SHAPE_POS_A[i];
      }
      break;
    case Z_SHAPE:
      tet->max_positions = 2;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case Z_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = Z_SHAPE_POS_A[i];
          }
          break;
        case Z_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = Z_SHAPE_POS_B[i];
          }
          break;
      }
      break;
    case T_SHAPE:
      tet->max_positions = 4;
      if (position == -1) { // randomise
        tet->position = _random(tet->max_positions);
      }
      switch(tet->position) {
        case T_SHAPE_POS_1:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = T_SHAPE_POS_A[i];
          }
          break;
        case T_SHAPE_POS_2:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = T_SHAPE_POS_B[i];
          }
          break;
        case T_SHAPE_POS_3:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = T_SHAPE_POS_C[i];
          }
          break;
        case T_SHAPE_POS_4:
          for(i = 0; i < 16; i++) {
            *(tet->data+i) = T_SHAPE_POS_D[i];
          }
          break;
      }
      break;
  }
}

// sets the tetromino position to the next possible one.
// if the next position is over the max available for that shape, it cycles back to the
// first position.
void tetromino_setNextPosition(Tetromino *tet) {
  if (tet->max_positions == 1) { // the O shape has only one position, this handles that
    return;
  }

  // if we can still increment postions
  if (tet->position < (tet->max_positions - 1)) {
    tet->position++;
  }
  else { // otherwise we set it back to the first position
    tet->position = 0;
  }
}

// sets the tetromino position to the previous one.
// if the next position is over the max available for that shape, it cycles back to the
// first position.
void tetromino_setPrevPosition(Tetromino *tet) {
  if (tet->max_positions == 1) { // the O shape has only one position, this handles that
    return;
  }

  // if we can still increment postions
  if (tet->position > 0) {
    tet->position--;
  }
  else { // otherwise we set it back to the first position
    tet->position = tet->max_positions - 1;
  }
}

void tetromino_moveLeft(Tetromino *tet) {
	if (tet) {
		tet->x--;
	}
}

void tetromino_moveRight(Tetromino *tet) {
	if (tet) {
		tet->x++;
	}
}

void tetromino_moveDown(Tetromino *tet) {
	if (tet) {
		tet->y++;
	}
}

void tetromino_moveUp(Tetromino *tet) {
	if (tet) {
		tet->y--;
	}
}

