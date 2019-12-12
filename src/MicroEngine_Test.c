#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroEngine/ME_ParticleSystem.h"

ME_ParticleSystem *part;

void HandleEvents(SDL_Event event)
{
    switch(event.type)
    {
    case SDL_MOUSEMOTION:
        part->systemPos.x =  event.motion.x;
        part->systemPos.y =  event.motion.y;
    }

}

void Update(float deltaTime)
{
    ME_UpdateParticleSystem(part, deltaTime);
}

void Render(SDL_Renderer *renderer)
{
    ME_RenderParticleSystem(part, renderer);
}


int main(int argc, char *argv[])
{
    if(!ME_Init("MicroEngine test",800,600))
    {
        ME_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = ME_GetRenderer();

    part = ME_CreateParticleSystem(400,300, 100);
    part->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Coin.png");

    ME_Run(&HandleEvents, &Update, &Render);

    ME_DestroyParticlSystem(part);

    ME_Quit();
    return 0;
}
