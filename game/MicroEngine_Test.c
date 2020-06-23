#include "../src/MicroEngine.c"

global ME_ParticleSystem *part = NULL;

void HandleEvent(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        part->systemPos.x = event.motion.x;
        part->systemPos.y = event.motion.y;
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
    Game game = ME_CreateGame("MicroEngine test", 800, 600);
    SDL_Renderer *renderer = game.platform.renderer;

    // ME_Init("MicroEngine test", 800,600);
    part = ME_CreateParticleSystem(400, 300, 100);
    part->texture = IMG_LoadTexture(renderer, "assets/Sprites/Coin.png");

    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    ME_RunGame(&game);

    // ME_Run(handleEvent, update, render);
    ME_DestroyParticlSystem(part);
    ME_QuitGame(&game);
    return 0;
}
