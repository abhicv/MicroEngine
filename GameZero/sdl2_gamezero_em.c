#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "sdl2_gamezero.h"

#ifdef __EMSCRIPTEN__
#include "gamezero.c"
#include <emscripten.h>
#endif

f32 deltaTime = 1.0f / 60.0f;
f32 targetDeltaTime = 1.0f/ 60.0f;
u64 performanceFreq = 0;

u64 startTime = 0;
u64 endTime = 0;
b32 quitGame = false;

void GameLoop()
{
    startTime = SDL_GetPerformanceCounter();
    
    while(SDL_PollEvent(&event))
    {
        HandleEvent(&event);
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    
    GameUpdateAndRender(&gameMode, &quitGame, deltaTime, renderer);
    
    SDL_RenderPresent(renderer);
    
    endTime = SDL_GetPerformanceCounter();
    u64 elapsed = (endTime - startTime) * 1000 / performanceFreq;
    deltaTime = (float)elapsed / 1000.0f;
    
    //printf("dt: %f\n", deltaTime);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    
    TTF_Init();
    
    window = SDL_CreateWindow("GameZero", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    performanceFreq = SDL_GetPerformanceFrequency();
    
    gameResource.playerSprite =  IMG_LoadTexture(renderer, "bin/data/levels/Player.png");
    gameResource.enemySprite =  IMG_LoadTexture(renderer, "bin/data/levels/enemies.png");
    gameResource.itemSprite = IMG_LoadTexture(renderer, "bin/data/levels/items.png");
    
    gameResource.tileSheetSurface =  IMG_Load("bin/data/levels/tiles.png");
    
    gameResource.coinSound = ME_LoadAudio("bin/data/audio/coin_collect.wav");
    gameResource.shootSound = ME_LoadAudio("bin/data/audio/shoot.wav");
    gameResource.jumpSound = ME_LoadAudio("bin/data/audio/jump.wav");
    gameResource.hurtSound = ME_LoadAudio("bin/data/audio/hurt.wav");
    
    gameResource.loaded = true;
    
    LoadData(renderer);
    
    ui.fontFile = "bin/data/levels/agoestoesan.ttf";
    
    emscripten_set_main_loop(GameLoop, 0, 1);
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
