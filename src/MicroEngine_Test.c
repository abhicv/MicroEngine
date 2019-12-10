#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroEngine/ME_ParticleSystem.h"

int main(int argc, char *argv[])
{
    if(!ME_Init("MicroEngine test",800,600))
        return 1;

    ME_ParticleSystem *part = ME_CreateParticleSystem(400,300, 100);
    part->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Coin.png");

    SDL_Renderer *renderer = ME_GetRenderer();

    bool quit = false;
    float deltaTime = 0.016f;
    SDL_Event event;

    while(!quit)
    {
        ME_GetDeltaTime(&deltaTime);

        if(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
                part->systemPos.x =  event.motion.x;
                part->systemPos.y =  event.motion.y;

            }
        }

        ME_UpdateParticleSystem(part, deltaTime);


        SDL_RenderClear(renderer);

        ME_RenderParticleSystem(part, renderer);

        SDL_RenderPresent(renderer);
    }

    ME_DestroyParticlSystem(part);

    ME_Quit();
    return 0;
}
