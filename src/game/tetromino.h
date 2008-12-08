#ifndef TETROMINO_H
#define TETROMINO_H

#define J_SHAPE 0
#define J_SHAPE_POS_1 0
#define J_SHAPE_POS_2 1
#define J_SHAPE_POS_3 2
#define J_SHAPE_POS_4 3

#define L_SHAPE 1
#define L_SHAPE_POS_1 0
#define L_SHAPE_POS_2 1
#define L_SHAPE_POS_3 2
#define L_SHAPE_POS_4 3

#define I_SHAPE 2
#define I_SHAPE_POS_1 0
#define I_SHAPE_POS_2 1

#define S_SHAPE 3
#define S_SHAPE_POS_1 0
#define S_SHAPE_POS_2 1

#define O_SHAPE 4

#define Z_SHAPE 5
#define Z_SHAPE_POS_1 0
#define Z_SHAPE_POS_2 1

#define T_SHAPE 6
#define T_SHAPE_POS_1 0
#define T_SHAPE_POS_2 1
#define T_SHAPE_POS_3 2
#define T_SHAPE_POS_4 3

// Tetromino Events
#define TETROMINO_CREATE SDL_USEREVENT+6

typedef struct _tetromino {
  int x, y;     // where on the grid the tetromino is
  int type;     // type of tetromino shape
  int position; // what shape position the tetromino is currently in
  int max_positions; // maximum positions tetromino can have
  unsigned int* data;
} Tetromino;

Tetromino *tetromino_createNew();
void tetromino_destroy(Tetromino*);
void tetromino_setShape(Tetromino*, int, int);
void tetromino_setNextPosition(Tetromino*);
void tetromino_setPrevPosition(Tetromino*);
void tetromino_moveLeft(Tetromino*);
void tetromino_moveRight(Tetromino*);
void tetromino_moveDown(Tetromino*);
void tetromino_moveUp(Tetromino*);

#endif

