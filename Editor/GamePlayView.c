#include "MicroEditor.h"

typedef void (*GameHandleEventFuncPtr)(SDL_Event *event);
typedef void (*GameUpdateAndRenderFuncPtr)(u32 *gameMode, bool *quitGame, f32 deltaTime, SDL_Renderer *renderer);
typedef void (*GameLoadLevelDataFuncPtr)(const char *entityMapFile, const char *tileMapFile, const char *collisionMapFile, SDL_Renderer *renderer);

void GameTestLoop(SDL_Renderer *renderer, u32 *appMode)
{
    HANDLE gameCode = LoadLibraryA("gamezero.dll");

    if(!gameCode)
    {
        printf("Failed to load gamezero.dll !\n");
        (*appMode) = APP_MODE_EDITOR;
        return;
    }

    GameLoadLevelDataFuncPtr gameLoadLevelData = (GameLoadLevelDataFuncPtr)GetProcAddress(gameCode, "LoadLevelData");
    GameHandleEventFuncPtr gameHandleEvent = (GameHandleEventFuncPtr)GetProcAddress(gameCode, "HandleEvent");
    GameUpdateAndRenderFuncPtr gameUpdateAndRender = (GameUpdateAndRenderFuncPtr)GetProcAddress(gameCode, "GameUpdateAndRender");

    gameLoadLevelData("tmp.emap", "tmp.tmap", "tmp.cmap", renderer);

    f32 deltaTime = 1.0f / 60.0f;
    
    u64 performanceFreq = SDL_GetPerformanceFrequency();
    
    bool quit = false;

    SDL_Event event = {0};

    while(!quit)
    {
        u64 startTime = SDL_GetPerformanceCounter();
                
        while(SDL_PollEvent(&event) > 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                (*appMode) = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    (*appMode) = APP_MODE_EDITOR;
                }
                break;
            }
            gameHandleEvent(&event);
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        gameUpdateAndRender(0, 0, deltaTime, renderer);
        SDL_RenderPresent(renderer);

        u64 endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / performanceFreq;
        deltaTime = (float)elapsed / 1000.0f;
    }
    
    FreeLibrary(gameCode);
}