#ifndef FONT_H
#define FONT_H

#include "SDL/SDL.h"

// padding for both horizontal and vertical
#define GLYPH_PADDING 1

typedef struct _glyph {
	int x, y; // top left point
	int w, h; // width and height
	void *next;
} Glyph;

int font_init();
void font_destroy();
int font_openFont(const char*, int);
int _createFontTexture();
int font_determineDimensions(int, int, int *, int *, int *);
void font_closeFont();
int font_getMaxGlyphWidth(int, int);
void font_print(float, float, const char*, SDL_Color *);

#endif
