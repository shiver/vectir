#include "particle.h"

#include "../util/common.h"

static ParticleCollection *_particles = NULL;
static int num_particle_collections = 0;

extern SDL_sem *sem;

Particle *particle_createNewParticle() {
  Particle *particle;

	debug_print("Creating particle...");
  particle = calloc(1, sizeof(Particle));
  if (particle == NULL) {
    error_print("Failed to allocate memory for particle.");
		return NULL;
  }
  particle->next = NULL;
  particle->collection = NULL;

  return particle;
}


void particle_createNewCollection(Particle *first) {
  ParticleCollection *current = NULL;
  Particle *current_particle;

	SDL_SemWait(sem);

  debug_print("Creating particle collection...");
  if (_particles) {
    current = _particles;
    while (current->next) {
      current = current->next;
    }
    current->next = malloc(sizeof(ParticleCollection));
    if (!_particles) {
      error_print("Unable to allocate memory for Particle Collection.");
			SDL_SemPost(sem);
      return;
    }
    ((ParticleCollection*)current->next)->first = first;
    ((ParticleCollection*)current->next)->next = NULL;

    current = current->next;

    current_particle = current->first;
    current->num_particles = 0;
    while (current_particle) {
      current->num_particles++;
      current_particle->collection = current;
      current_particle = current_particle->next;
    }
  }
  else {
    _particles = malloc(sizeof(ParticleCollection));
    if (!_particles) {
      error_print("Unable to allocate memory for Particle Collection.");
			SDL_SemPost(sem);
      return;
    }

    _particles->first = first;
    _particles->next = NULL;

    current = _particles;

    current_particle = current->first;
    current->num_particles = 0;
    while (current_particle) {
      current->num_particles++;
      current_particle->collection = current;
      current_particle = current_particle->next;
    }
  }

  num_particle_collections++;
	SDL_SemPost(sem);
}

void particle_destroyCollection(ParticleCollection *collection) {
  ParticleCollection *current = NULL;
  ParticleCollection *previous = NULL;
  ParticleCollection *next = NULL;

  if (_particles == collection) {
    if (_particles->next) {
      next = _particles->next;

      debug_print("Deleting particle collection...");
      free(_particles);

      _particles = next;
    }
    else {
      debug_print("Deleting particle collection...");
      free(_particles);
      _particles = NULL;
    }
    num_particle_collections--;
  }
  else {
		previous = _particles;
    current = _particles->next;

    while (current) {
      if (current == collection) {
        if (previous && current->next) {
          previous->next = current->next;
          debug_print("Deleting particle collection...");
          free(current);
          num_particle_collections--;
          return;
        }
        else if (previous && previous->next) {
          previous->next = NULL;
          debug_print("Deleting particle collection...");
          free(current);
          num_particle_collections--;
          return;
        }
      }
			previous = current;
      current = current->next;
    }
  }
}

void particle_destroy(Particle *particle) {
	// wait for the all clear
	SDL_SemWait(sem);

  if (particle != NULL) {
    if (particle->collection) {
      ((ParticleCollection*)particle->collection)->num_particles--;
    }

    // check if this is the last particle in the collection
    if (((ParticleCollection*)particle->collection)->num_particles == 0) {
      particle_destroyCollection(particle->collection);
    }

    debug_print("Deleting particle");
    free(particle);
    particle = NULL;
  }

	// give the all clear
	SDL_SemPost(sem);
}

ParticleCollection *particle_getActiveCollections() {
  return _particles;
}

// TODO: Change so that collections are only deleted after all are processed
void particle_update() {
	int active = 0;
	Particle *first = NULL;
	Particle *current = NULL;
	Particle *previous = NULL;
	Particle *next = NULL;
	ParticleCollection *collection = NULL;
	
	// get a list of collections which need to be updated
	collection = particle_getActiveCollections();
	while (collection) {
		first = collection->first;

	  if (first) {
		  current = first;

	    while (current) {
	      current->y += current->v_speed;
		    current->alpha += current->alpha_decay;

				if (current->alpha > 0) {
					active = 1;
				}

				previous = current;
				current = current->next;
			}

			if (!active) {
				current = first;
				while (current) {
					next = current->next;
					particle_destroy(current);
					current = next;
				}

				first = NULL;
				current = NULL;
			}
		}

		collection = collection->next;
	}
}

