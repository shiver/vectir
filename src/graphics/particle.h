#ifndef PARTICLE_H
#define PARTICLE_H

#include "SDL/SDL.h"

#include "graphics.h"

#define PARTICLE_UPDATE_INTERVAL 30

typedef struct _particle {
  Texture *texture;
  SDL_Color color;
  float x, y;
  float alpha_decay;
  float alpha;
  float h_accel, v_accel;
  float h_speed, v_speed;
  void *next;
  int id;
  void *collection;
} Particle;

typedef struct _particleCollection {
  Particle *first;
  void *next;
  int num_particles;
} ParticleCollection;

Particle *particle_createNew();
Particle *particle_createNewParticle();
void particle_createNewCollection(Particle *);
ParticleCollection *particle_getActiveCollections();
void particle_destroy(Particle*);
void particle_update();

#endif
