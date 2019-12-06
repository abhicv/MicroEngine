#ifndef ME_PARTICLESYSTEM_H
#define ME_PARTICLESYSTEM_H

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "ME_Vector2D.h"

typedef struct {

    int numParticles;
    Vector2 systemPos;
    Vector2 *positions;
    Vector2 *velocity;
    SDL_Texture *texture;

}ME_ParticleSystem;

ME_ParticleSystem* ME_CreateParticleSystem(int x, int y, int numParticles);
void ME_UpdateParticleSystem(ME_ParticleSystem *particleSystem, float deltaTime);
void ME_RenderParticleSystem(ME_ParticleSystem *particleSystem, SDL_Renderer *renderer);
void ME_DestroyParticlSystem(ME_ParticleSystem *particleSystem);


#endif // ME_PARTICLESYSTEM_H
