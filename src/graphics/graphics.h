#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL.h"

// holds information abotu a particular texture
typedef struct _texture {
  char *filename;
	int w, h;	    // width and height
  float tw, th; // texture width and height
	int id;			  // OpenGL texture id
} Texture;

// a collection of texture objects
typedef struct _textureCollection {
  Texture *first;
  Texture *current;
} TextureCollection;

// used to access all textures
TextureCollection textures;

int power_of_two(int);
void graphics_init();
void graphics_clear();
void graphics_drawTexture(int, int, Texture*);
void graphics_drawTextureScaled(int, int, int, int, Texture*);
void graphics_drawBox(int, int, int, int);
void graphics_drawGrid(int, int, int, int, int, SDL_Color*);
void graphics_radialBlur(int, int, int, int, Texture*);
void graphics_destroyTexture(Texture*);
void graphics_shutdown();

Texture *graphics_createEmptyTexture(int, int);
Texture *graphics_createTextureFromViewport(int, int, int, int);
Texture *graphics_createTextureFromFile(const char *);
Texture *graphics_createTextureFromSurface(SDL_Surface *);
void graphics_adjustTextureFromViewport(int, int, int, int, Texture*);


#endif

