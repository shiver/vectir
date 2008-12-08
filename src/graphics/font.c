// TODO (robertva#1#): Text coloring
// TODO (robertva#2#): Review font code and structure
#include "font.h"

#include "../util/common.h"
#include "graphics.h"

#include "SDL/SDL_ttf.h"
#include "SDL/SDL_opengl.h"

#include <math.h>

static TTF_Font *font;
static Texture *font_texture;
static int font_base;

int font_init() {
  // initialise TTF system
  if (TTF_Init() == -1) {
    error_print("Failed to initialise TTF system.");
    return 0;
  }
  return 1;
}

// shutdown the TTF system
void font_destroy() {
  TTF_Quit();
  if (font) {
    font_closeFont();
  }
}

// opens a ttf font with the specified size
int font_openFont(const char *filename, int size) {
  font = TTF_OpenFont(filename, size);
  if (!font) {
    error_print("Failed to open font.");
    return 0;
  }

  // creates the texture for the font
  debug_print("Creating font texture...");
  _createFontTexture();

  return 1;
}

Glyph *font_getGlyphInfo(int start, int count) {
	int i;
	Glyph *first;
	Glyph *current;
	int minx, maxx, miny, maxy;

	first = malloc(sizeof(Glyph));
	if (!first) {
		error_print("Unable to allocate memory for glyph info.");
		return NULL;
	}

	current = first;

	for (i = 0; i < (start + count); i++) {
		TTF_GlyphMetrics(font, start + count, &minx, &maxx, &miny, &maxy, NULL);
		current->w = maxx - minx;
		current->h = maxy - miny;
		current->x = 0;
		current->y = 0;
		current->next = NULL;

		if (i + 1 < (start + count)) {
			current->next = malloc(sizeof(Glyph));
			if (!current->next) {
				error_print("Unable to allocate memory for glyph info.");
				// todo: add destruction code here
				return NULL;
			}
			current = current->next;
		}
	}

	return first;
}

void font_destroyGlyphInfo(Glyph *first) {
	Glyph *current;
	Glyph *next;

	if (!first) {
		return;
	}

	// set what we will look at next
	current = first->next;
	// free the first glyph info
	free(first);

	// now loop until all glyphs are freed
	while (current) {
		if (current->next) {
      next = current->next;
		}
		else {
      next = NULL;
		}

		free(current);

		current = next;
	}
}

// creates the texture for the font
// TODO (robertva#1#): Handle glyph sizes better. Each glyph has a unique width
// + height...
int _createFontTexture() {

  SDL_Surface *surface;
  SDL_Surface *surface_texture;
  SDL_Rect srcrect, dstrect;
  int start = 32;
  int count = 94;
  int x, y;
  float xval, yval;
  char c[2];

  int tw, th;
	Glyph *first, *current, *row;
	int per_row;
	int width, height;
	SDL_Color color_fg, color_bg;

	// set vars
	memset(c, '\0', sizeof(c));
	per_row = 0;
  width = 0;
  height = 0;
	debug_print("Getting glyph info.");
	first = font_getGlyphInfo(start, count);
	first->x = 0;
	first->y = 0;
	current = first;
	row = current;

	// todo: make colour settable
  color_fg.r = 255;
  color_fg.g = 255;
  color_fg.b = 255;
  color_bg.r = 0;
  color_bg.g = 0;
  color_bg.b = 0;

  if (!font) {
    error_print("_createFontTexture: Font not open.");
    return 0;
  }

  // determine the dimensions required to fit all glyphs and
	// number of characters in a row
	debug_print("Getting font texture dimensions.");
	per_row = round(sqrt(count)); // rounds up. remember to cater for fewer characters
	font_determineDimensions(start, count, &tw, &th, &per_row);
	// set texture width and height to nearest power of two
	tw = power_of_two(tw);
	th = power_of_two(th);
	printf("dims: %d %d\n", tw, th);

  // generate a place for the final surface
	debug_print("Creating new surface for font texture.");
  surface_texture = SDL_CreateRGBSurface(SDL_SWSURFACE, tw, th, 32, 0, 0, 0, 0);
  if (!surface_texture) {
    error_print("Failed to create new surface for font.");
    return 0;
  }

  for (y = 0; y < per_row; y++) {
		// store the glyph at the beginning of the row
		row = current;
		height = 0;

		// need to first get the max height out of the glyphs in this row
		for (x = 0; x < per_row; x++) {
			if (current) {
				if (current->h + (GLYPH_PADDING * 2) > height) {
					height = current->h + (GLYPH_PADDING * 2);
				}
			}
			current = current->next;
		}

		// go back to the beginning of the row
		current = row;

		// now do the actual rendering and applying to the finial surface
    for (x = 0; x < per_row; x++) {
			if (x + (y * per_row) < count) { // correction for rounding of per_row
				if (current) {
					// render next character
		      c[0] = start + x + (y * per_row);
		      surface = TTF_RenderText_Shaded(font, c, color_fg, color_bg);

					current->w = surface->w;
					current->h = surface->h;

					// set the src dimensions
					srcrect.x = 0;
					srcrect.y = 0;
					srcrect.w = surface->w;
					srcrect.h = surface->h;

					// set the destination location for the new char
			    dstrect.x = current->x + GLYPH_PADDING;
				  dstrect.y = current->y + GLYPH_PADDING;
					dstrect.w = surface->w;
					dstrect.h = current->h;

				  // copy the row to the final surface
					SDL_BlitSurface(surface, &srcrect, surface_texture, &dstrect);

					SDL_FreeSurface(surface);

					// set the top left corner of the next glyph
					if (current->next) {
						if (x + 1 < per_row) { // if we're still on the same row
							((Glyph*)current->next)->x = current->x + current->w + (GLYPH_PADDING * 2);
							((Glyph*)current->next)->y = current->y;
						}
						else { // on the next row
							((Glyph*)current->next)->x = 0;
							((Glyph*)current->next)->y = current->y + current->h + (GLYPH_PADDING * 2);
						}
					}
					current = current->next;
				}
			}
    }
  }

  // create an opengl texture from the surface
  font_texture = graphics_createTextureFromSurface(surface_texture);
  //SDL_SaveBMP(surface_texture, "font.bmp"); // used for debugging
  SDL_FreeSurface(surface_texture);

  // reset the glyph info list
  current = first;

  debug_print("Generating font display lists...");
  font_base = glGenLists(count); // create opengl display list

  for (y = 0; y < per_row; y++ ) {
    for (x = 0; x < per_row; x++ ) {
      if (x + (y * per_row) < count) {
        if (current) {
          glNewList(font_base + x + (y * per_row), GL_COMPILE); // new char
            glBegin(GL_QUADS);
              // texture 0, 0
              xval = current->x / (float)tw;
              yval = current->y / (float)th;
              glTexCoord2f(xval, yval);
              glVertex2i(0, 0);

              // texture 1, 0
              xval = ((current->x + current->w + (GLYPH_PADDING * 2)) / (float)tw);
              yval = current->y / (float)th;
              glTexCoord2f(xval, yval);
              glVertex2i(current->w + (GLYPH_PADDING * 2), 0);

              // texture 1, 1
              xval = ((current->x + current->w + (GLYPH_PADDING * 2)) / (float)tw);
              yval = (current->y / (float)th) + (current->h / (float)th);
              glTexCoord2f(xval, yval);
              glVertex2i(current->w + (GLYPH_PADDING * 2), current->h);

              // texture 0, 1

              xval = current->x / (float)tw;
              yval = (current->y / (float)th) + (current->h / (float)th);
              glTexCoord2f(xval, yval);
              glVertex2i(0, current->h);
            glEnd();
            glTranslatef(current->w + (GLYPH_PADDING), 0, 0.0f); // move to next place
          glEndList();

          current = current->next;
        }
      }
    }
  }

	font_destroyGlyphInfo(first);
	return 1;
}

// close font object
void font_closeFont() {
  TTF_CloseFont(font);
  if (font_base > 0) {
    glDeleteLists(font_base, 96); // todo: change 96
  }

  if (font_texture) {
    graphics_destroyTexture(font_texture);
  }
  font = NULL;
}

// determines the dimensions to create the most economical texture size,
// based on the font starting char and count. glyphs are organised into a 2d
// array
// returns dimensions in *width and *height
// todo: change to use font_getGlyphInfo()
int font_determineDimensions(int start_char, int count, int *width, int *height, int *per_row) {
  int minx, maxx, miny, maxy, advance;
  int cur_row_width, max_row_width;
	int cur_max_height;
  int glyphs[*per_row][*per_row];
  int x, y;

  // set to 0
  *width = 0;
  *height = 0;
	max_row_width = 0;

	// cycle through all characters starting with start_char
  for (y = 0; y < *per_row; y++) {
    // reset for the next row
		cur_row_width = 0;
		cur_max_height = 0;

    for (x = 0; x < *per_row; x++ ) {
      if ((x+(*per_row * y)) < count) { // make sure we don't do too many chars (due to the rounding above)
				glyphs[x][y] = start_char + (x + (x * y));

        // get the metrics
        if (TTF_GlyphMetrics(font, glyphs[x][y], &minx, &maxx, &miny,
            &maxy, &advance)){
          error_print("Failed to get Glyph metrics.");
          return 0;
        }

				// increment the row width by the glyph width
				cur_row_width += advance + (GLYPH_PADDING * 2);

				// is this the highest character in the row
				if (maxy + (GLYPH_PADDING * 2) > cur_max_height) {
					cur_max_height = maxy + (GLYPH_PADDING * 2) + 20; // todo: 20 is a temp fix...
				}
      }
    }
    *height += cur_max_height; // use highest char as row height and add to total

		// is this the widest row so far?
		if (cur_row_width > max_row_width)
      max_row_width = cur_row_width + 20; // todo: 20 is a temp fix...
  }

	*width = max_row_width; // set the widest row as the width

  return 1;
}

// print the text
void font_print(float x, float y, const char *text, SDL_Color *color) {
  int i;

  // save state
  glPushMatrix();

	glPushAttrib(GL_CURRENT_BIT); // color

	// set the color of the font (if color is not NULL)
	if (color != NULL) {
		glColor4f(color->r/255, color->g/255, color->b/255, 1.0f);
	}
	else { // otherwise, plain white
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

  // enable and bind the font texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, font_texture->id);

  glLoadIdentity();
  glTranslatef(x, y, 0.0f);
  // set starting point of display list
  glListBase(font_base-32);

  for (i = 0; i < strlen(text); i++ ) {
    glCallLists(font_base, GL_UNSIGNED_BYTE, text+i);
  }
  // go back to how we were
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);

	glPopAttrib(); // Color
  glPopMatrix();
}
