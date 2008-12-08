#include "graphics.h"

#include "../util/common.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_thread.h"

SDL_sem *texture_sem;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

int power_of_two(int val) {
  int i;

  val--;
  for (i = 1; i < sizeof(val) * 8; i = i * 2) {
    val = val | val >> i;
  }
  return val + 1;
}

// returns 1 if value is a power of two, else 0
int isPowerOfTwo(int val) {
  if (val == 0) // 0 is not a power of two
    return 0;

  if ((val & (val - 1)) == 0)  // is it a power of two?
    return 1;
  else
    return 0;
}

void graphics_drawTexture(int x, int y, Texture *texture) {
	SDL_SemWait(texture_sem); 

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, texture->tw);        glVertex2i(x, y);
	glTexCoord2f(texture->tw, texture->th); glVertex2i(x+texture->w, y);
	glTexCoord2f(texture->tw, 0.0f);        glVertex2i(x+texture->w, y+texture->h);
	glTexCoord2f(0.0f, 0.0f);               glVertex2i(x, y+texture->h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	SDL_SemPost(texture_sem);
}

// draws a texture but with the specified width and height
void graphics_drawTextureScaled(int x, int y, int w, int h, Texture *texture) {
	SDL_SemWait(texture_sem);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, texture->tw);        glVertex2i(x, y);
	glTexCoord2f(texture->tw, texture->th); glVertex2i(x+w, y);
	glTexCoord2f(texture->tw, 0.0f);        glVertex2i(x+w, y+h);
	glTexCoord2f(0.0f, 0.0f);               glVertex2i(x, y+h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture

	SDL_SemPost(texture_sem);
}

void graphics_clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void graphics_drawBox(int x, int y, int w, int h) {
	glPushAttrib(GL_CURRENT_BIT); // color

	glColor4f(1.0f, 1.0f, 1.0f, 0.20f);

	glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
	glEnd();

	glPopAttrib();
}

// TODO: Make real time radial blurring work
void graphics_radialBlur(int x, int y, int w, int h, Texture *texture) {
	float zoom = 0.0f;
	float alpha = 0.5f;
	int i;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, w, h, 0);

//	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	glBegin(GL_QUADS);
	for (i = 0; i < 25; i++) {
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glTexCoord2f(0-zoom, 1-zoom); glVertex2f(x, y);
		glTexCoord2f(1+zoom, 1-zoom); glVertex2f(x+w, y);
		glTexCoord2f(1+zoom, 0+zoom); glVertex2f(x+w, y+h);
		glTexCoord2f(0-zoom, 0+zoom); glVertex2f(x, y+h);

		zoom += 1.0f / 25;
		alpha -= 0.05f;
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

// create an empty texture with the specified width and height
Texture *graphics_createEmptyTexture(int w, int h) {
	unsigned int *data = NULL;
	Texture *texture;

	data = calloc(1, ((w * h) * sizeof(unsigned int) * 4));
	if (!data) {
		error_print("Failed to allocate memory for empty texture.");
		return NULL;
	}

	texture = calloc(1, sizeof(Texture));
	if (!texture) {
		error_print("Failed to allocate memory for texture object.");
		return NULL;
	}

	texture->filename = NULL;
	texture->w = w;
	texture->h = h;
	texture->tw = 1.0f;
	texture->th = 1.0f;

	glGenTextures(1, &texture->id);

	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w,
		h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(data);
	return texture;
}

Texture *graphics_createTextureFromViewport(int x, int y, int w, int h) {
  Texture *texture;

  texture = calloc(1, sizeof(texture));
  if (!texture) {
    error_print("Failed to allocate memory for texture object.");
    return NULL;
  }
  texture->filename = NULL;

  // create a new empty texture
  texture = graphics_createEmptyTexture(w, h);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // having GL_RGBA breaks the ability to adjust the texture alpha
  // through glColor4f. why?
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, w, h, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);

  return texture;
}

void graphics_adjustTextureFromViewport(int x, int y, int w, 
		int h, Texture *texture) {
	
	if (!texture) {
		error_print("Failed to adjust texture to match viewport.");
		return;
	}
	
	SDL_SemWait(texture_sem);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // having GL_RGBA breaks the ability to adjust the texture alpha
  // through glColor4f. why?
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, w, h, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);

	SDL_SemPost(texture_sem);
}

// TODO: adjust to not copy so much from createTextureFromFile
Texture *graphics_createTextureFromSurface(SDL_Surface *surface) {
  Texture *texture;
  SDL_Surface *nonTwoSurface;
  int w, h;
  int mode;

  // allocate memory for texture object
  texture = calloc(1, sizeof(Texture));

  if (texture == NULL) {
    error_print("Failed to allocate memory for texture object.");
    exit(1);
  }

  if (!surface) { // if image load fails
		error_print("Surface not supplied.");
    graphics_destroyTexture(texture);
    return NULL;
	}

	texture->filename = NULL;

  // set format of the surface
  if (surface->format->BytesPerPixel == 3) {
    mode = GL_RGB;
  }
  else if (surface->format->BytesPerPixel == 4) {
    mode = GL_RGBA;
  }
  else {  // jump ship if can't handle the format
    error_print("Unknown image format type.");
    graphics_destroyTexture(texture);
    return NULL;
  }

  // set texture object values
  texture->w = surface->w;
  texture->h = surface->h;

  // check if image is a power of two
  if (isPowerOfTwo(texture->w) && isPowerOfTwo(texture->h)) {
    debug_print("Power-of-two image.");

    w = texture->w;
    h = texture->h;
    texture->tw = 1.0f;
    texture->th = 1.0f;
  }
  else { // Do some mangling to get non-power of two images working
    debug_print("Non-power-of-two image.");
    w = power_of_two(surface->w);
    h = power_of_two(surface->h);

    texture->tw = texture->w / (float)w;
    texture->th = texture->h / (float)h;

    // create new surface with the correct dimensions
    nonTwoSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
      surface->format->BitsPerPixel, surface->format->Rmask,
      surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    if (nonTwoSurface == NULL) {  // check is surface was created, return null if not
      error_print("Failed to create nonTwoSurface.");
      graphics_destroyTexture(texture);
      return NULL;
    }
    debug_print("New surface created.");

    // copy original image data onto new surface
    SDL_BlitSurface(surface, NULL, nonTwoSurface, NULL);
    debug_print("Src blitted to dst.");

    // free the original surface and assign to the new one
    SDL_FreeSurface(surface);
    surface = nonTwoSurface;
  }

  printf("w:%f, h:%f\n", texture->tw, texture->th);


  // opengl create texture
  glGenTextures(1, &texture->id);

  glBindTexture(GL_TEXTURE_2D, texture->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /*gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture->w,
    texture->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);*/
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w,
    surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

  glBindTexture(GL_TEXTURE_2D, 0); // unbind
  return texture;
}

// creates an opengl texture from the file supplied
Texture *graphics_createTextureFromFile(const char *filename) {
	SDL_Surface *surface;
  SDL_Surface *nonTwoSurface;
  Texture *texture;
  int w, h;
  int mode;

  // allocate memory for texture object
  texture = malloc(sizeof(Texture));

  if (texture == NULL) {
    error_print("Failed to allocate memory for texture object.");
    exit(1);
  }
  // zero the struct
  memset(texture, '\0', sizeof(Texture));

  texture->filename = malloc(sizeof(char) * strlen(filename));
  if (texture->filename == NULL) {
    error_print("Failed to allocate memory for texture filename.");
    exit(1);
  }

  surface = IMG_Load(filename);
	if (!surface) { // if image load fails
		error_print("Failed to load image.");
		return NULL;
	}

  // set format of the texture
  if (surface->format->BytesPerPixel == 3) {
    mode = GL_RGB;
  }
  else if (surface->format->BytesPerPixel == 4) {
    mode = GL_RGBA;
  }
  else {  // jump ship if can't handle the format
    SDL_FreeSurface(surface);
    error_print("Unknown image format type.");
    free(texture->filename);
    free(texture);
    return NULL;
  }

  // set texture object values
  texture->w = surface->w;
  texture->h = surface->h;

  // check if image is a power of two
  if (isPowerOfTwo(texture->w) && isPowerOfTwo(texture->h)) {
    debug_print("Power-of-two image.");

    w = texture->w;
    h = texture->h;
    texture->tw = 1.0f;
    texture->th = 1.0f;
  }
  else { // Do some mangling to get non-power of two images working
    debug_print("Non-power-of-two image.");
    w = power_of_two(surface->w);
    h = power_of_two(surface->h);

    texture->tw = texture->w / (float)w;
    texture->th = texture->h / (float)h;

    // create new surface with the correct dimensions
    nonTwoSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
      surface->format->BitsPerPixel, surface->format->Rmask,
      surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    if (nonTwoSurface == NULL) {  // check is surface was created, return null if not
      error_print("Failed to create nonTwoSurface.");
      SDL_FreeSurface(surface);
      free(texture->filename);
      free(texture);
      return NULL;
    }
    debug_print("New surface created.");

    // copy original image data onto new surface
    SDL_BlitSurface(surface, NULL, nonTwoSurface, NULL);
    debug_print("Src blitted to dst.");

    // free the original surface and assign to the new one
    SDL_FreeSurface(surface);
    surface = nonTwoSurface;
  }

  memcpy(texture->filename, filename, sizeof(char) * strlen(filename));
  printf("w:%f, h:%f\n", texture->tw, texture->th);


  // opengl create texture
  glGenTextures(1, &texture->id);

  glBindTexture(GL_TEXTURE_2D, texture->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /*gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture->w,
    texture->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);*/
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w,
    surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);

  return texture;
}

void graphics_drawGrid(int x, int y, int size, int width, int height, SDL_Color *color) {
	int i;

	glPushAttrib(GL_CURRENT_BIT);

	glColor4f(0.08f, 0.08f, 0.09f, 0.0f);
	glBegin(GL_LINES);
	// horizontal lines
	for (i = 0; i < height + 1; i++) {
		glVertex2i(x, y + (i * size));
		glVertex2i(x + (width * size), y + (i * size));
	}

	// vertical lines
	for (i = 0; i < width + 1; i++) {
		glVertex2i(x + (i * size), y);
		glVertex2i(x + (i * size), y + (height * size));
	}
	glEnd();

	glPopAttrib();
}

void graphics_init() {
	SDL_Surface *screen;

	debug_print("Starting SDL video subsystem...");

	// setup sdl video subsystem first
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error_print("Failed to init SDL video subsystem.");
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	// set video mode or nearest equal
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_OPENGL);
	if (screen == NULL) {
		error_print("Couldn't set desired video mode.");
		exit(1);
	}
	else {
		debug_print("Video mode set.");
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	debug_print("Viewport set.");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);
	debug_print("Ortho configured.");

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	debug_print("OpenGL attributes set.");

	// create a semaphore to limit access to the textures
	texture_sem = SDL_CreateSemaphore(1);
}

void graphics_destroyTexture(Texture *texture) {
	if (texture == NULL) {
		return;
	}

	if (texture->id) {
		glDeleteTextures(1, &texture->id);
	}

	if (texture->filename) {
		free(texture->filename);
	}

	free(texture);
}

void graphics_shutdown() {
	debug_print("Shutting down SDL video subsystem.");
	SDL_Quit();
}

