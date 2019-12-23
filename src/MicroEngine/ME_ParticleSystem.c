#include "../../include/MicroEngine/ME_ParticleSystem.h"
#include "../../include/MicroEngine/ME_Utility.h"


ME_ParticleSystem* ME_CreateParticleSystem(int x, int y, int numParticles)
{
    ME_ParticleSystem *particleSystem = (ME_ParticleSystem*)malloc(sizeof(ME_ParticleSystem));

    particleSystem->numParticles = numParticles;
    particleSystem->systemPos = NewVector2(x, y);

    particleSystem->texture = NULL;

    particleSystem->velocity = (Vector2*)malloc(sizeof(Vector2) * numParticles);
    particleSystem->positions = (Vector2*)malloc(sizeof(Vector2) * numParticles);

    int i = 0;
    int angle = 0;

    for(i = 0; i < particleSystem->numParticles; i++)
    {
        particleSystem->positions[i] = NewVector2(x, y);

        angle = rand() % 360;
        particleSystem->velocity[i] = NewVector2((rand() % 100) * cos(angle), (rand() % 100) * sin(angle));
    }

    SDL_Log("Particle system :: no of particles : %d", numParticles);

    return particleSystem;
}

void ME_UpdateParticleSystem(ME_ParticleSystem *particleSystem, float deltaTime)
{
    int i = 0;
    int x;
    int y;

    for(i = 0; i < particleSystem->numParticles; i++)
    {
        particleSystem->positions[i].x += particleSystem->velocity[i].x * deltaTime;
        particleSystem->positions[i].y += particleSystem->velocity[i].y * deltaTime;

        x = particleSystem->positions[i].x - particleSystem->systemPos.x;
        y = particleSystem->positions[i].y - particleSystem->systemPos.y;

        if(( x * x + y * y) > 200 * 200)
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
    int i = 0;
    SDL_Rect destRect = {0, 0, 20, 20};
    SDL_Color color = ME_HexToSdlColor(0xff00ff);

    for(i = 0; i < particleSystem->numParticles; i++)
    {
        destRect.x = particleSystem->positions[i].x;
        destRect.y = particleSystem->positions[i].y;

        //destRect.w = destRect.h = rand() % SDL_GetTicks() / 1000;

        // color = ME_RandomSdlColor();

        if(particleSystem->texture != NULL)
            SDL_RenderCopyEx(renderer, particleSystem->texture, NULL, &destRect, 0, NULL, SDL_FLIP_NONE);

        SDL_SetRenderDrawColor(renderer, rand() % 255,rand() % 255,rand() % 255,255);
        SDL_RenderDrawPoint(renderer, destRect.x, destRect.y);
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);

        ME_RenderDrawRect(renderer, &destRect, color);
    }
}

void ME_DestroyParticlSystem(ME_ParticleSystem *particleSystem)
{
    SDL_DestroyTexture(particleSystem->texture);
    free(particleSystem->velocity);
    free(particleSystem->positions);
    free(particleSystem);
}
