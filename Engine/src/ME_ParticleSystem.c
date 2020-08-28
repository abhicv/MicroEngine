#include "ME_ParticleSystem.h"
#include "ME_Utility.h"

ME_ParticleSystem *ME_CreateParticleSystem(i32 x, i32 y, u32 numParticles)
{
    ME_ParticleSystem *particleSystem = (ME_ParticleSystem *)malloc(sizeof(ME_ParticleSystem));
    
    particleSystem->numParticles = numParticles;
    particleSystem->systemPos = Vector2Init(x, y);
    particleSystem->texture = NULL;
    particleSystem->velocity = (Vector2 *)malloc(sizeof(Vector2) * numParticles);
    particleSystem->positions = (Vector2 *)malloc(sizeof(Vector2) * numParticles);
    
    u32 i = 0;
    f32 angle = 0.0f;
    
    for (u32 i = 0; i < particleSystem->numParticles; i++)
    {
        particleSystem->positions[i] = Vector2Init(x, y);
        angle = (f32)(rand() % 360);
        particleSystem->velocity[i] = Vector2Init((rand() % 100) * cos(angle), (rand() % 100) * sin(angle));
    }
    
    printf("Particle system :: no of particles : %d", numParticles);
    
    return particleSystem;
}

void ME_UpdateParticleSystem(ME_ParticleSystem *particleSystem, f32 deltaTime)
{
    i32 x = 0;
    i32 y = 0;
    
    for (u32 i = 0; i < particleSystem->numParticles; i++)
    {
        particleSystem->positions[i].x += particleSystem->velocity[i].x * deltaTime;
        particleSystem->positions[i].y += particleSystem->velocity[i].y * deltaTime;
        
        x = particleSystem->positions[i].x - particleSystem->systemPos.x;
        y = particleSystem->positions[i].y - particleSystem->systemPos.y;
        
        if ((x * x + y * y) >= (200 * 200))
        {
            particleSystem->positions[i].x = particleSystem->systemPos.x;
            particleSystem->positions[i].y = particleSystem->systemPos.y;
            
            //particleSystem->velocity[i].y = 100.0;
            //particleSystem->velocity[i].x = 0;
        }
    }
}

void ME_RenderParticleSystem(ME_ParticleSystem *particleSystem, SDL_Renderer *renderer)
{
    SDL_Rect destRect = {0, 0, 20, 20};
    SDL_Color color = ME_HexToSdlColor(0xff00ff);
    
    for (u32 i = 0; i < particleSystem->numParticles; i++)
    {
        destRect.x = particleSystem->positions[i].x;
        destRect.y = particleSystem->positions[i].y;
        //destRect.w = destRect.h = rand() % SDL_GetTicks() / 1000;
        // color = ME_RandomSdlColor();
        
        if (particleSystem->texture != NULL)
        {
            SDL_RenderCopyEx(renderer, particleSystem->texture, NULL, &destRect, 0, NULL, SDL_FLIP_NONE);
        }
        else 
        {
            SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, 255);
            SDL_RenderDrawPoint(renderer, destRect.x, destRect.y);
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        ME_RenderDrawRect(renderer, &destRect, color);
    }
}

void ME_DestroyParticlSystem(ME_ParticleSystem *particleSystem)
{
    SDL_DestroyTexture(particleSystem->texture);
    free(particleSystem->velocity);
    free(particleSystem->positions);
    free(particleSystem);
    
    particleSystem->velocity = NULL;
    particleSystem->positions = NULL;
    particleSystem = NULL;
}
