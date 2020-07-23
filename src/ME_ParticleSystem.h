#ifndef ME_PARTICLESYSTEM_H
#define ME_PARTICLESYSTEM_H

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "types.h"
#include "ME_Vector2D.h"

typedef struct
{
    u32 numParticles;
    Vector2 systemPos;
    Vector2 *positions;
    Vector2 *velocity;
    SDL_Texture *texture;

} ME_ParticleSystem;

internal ME_ParticleSystem *ME_CreateParticleSystem(i32 x, i32 y, u32 numParticles);

internal void ME_UpdateParticleSystem(ME_ParticleSystem *particleSystem, float deltaTime);

internal void ME_RenderParticleSystem(ME_ParticleSystem *particleSystem, SDL_Renderer *renderer);

internal void ME_DestroyParticlSystem(ME_ParticleSystem *particleSystem);

#endif // ME_PARTICLESYSTEM_H
