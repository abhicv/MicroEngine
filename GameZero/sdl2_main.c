#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <windows.h>

#include "gamezero.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event = {0};

bool quitGlobal = false;
u32 gameMode = GAME_MAIN_MENU;

typedef struct GameCode
{
    HMODULE gameCodeDLL;
    
    //NOTE(abhicv): Game Code functions loaded from DLL
    gameUpdateAndRender *GameUpdateAndRenderP;
    loadData *LoadDataP;
    handleEvent *HandleEventP;
    showMainMenu *ShowMainMenuP;
    showCredits *ShowCreditsP;
    
} GameCode;

GameCode LoadGameCode()
{
    GameCode gameCode = {0};
    gameCode.gameCodeDLL = LoadLibraryA("gamezero.dll");
    
    if(gameCode.gameCodeDLL)
    {
        gameCode.GameUpdateAndRenderP = (gameUpdateAndRender*)GetProcAddress(gameCode.gameCodeDLL, "GameUpdateAndRender");
        gameCode.LoadDataP = (loadData*)GetProcAddress(gameCode.gameCodeDLL, "LoadData");
        gameCode.HandleEventP = (handleEvent*)GetProcAddress(gameCode.gameCodeDLL, "HandleEvent");
        gameCode.ShowMainMenuP = (showMainMenu*)GetProcAddress(gameCode.gameCodeDLL, "ShowMainMenu");
        gameCode.ShowCreditsP = (showCredits*)GetProcAddress(gameCode.gameCodeDLL, "ShowCredits");
        
        bool gameCodeLoaded = gameCode.GameUpdateAndRenderP 
            && gameCode.LoadDataP
            && gameCode.HandleEventP
            && gameCode.ShowMainMenuP
            && gameCode.ShowCreditsP;
        
        if(!gameCodeLoaded)
        {
            gameCode.GameUpdateAndRenderP = GameUpdateAndRenderStub;
            gameCode.LoadDataP = LoadDataStub;
            gameCode.HandleEventP = HandleEventStub;
            gameCode.ShowMainMenuP = ShowMainMenuStub;
            gameCode.ShowCreditsP = ShowCreditsStub;
        }
    }
    
    return gameCode;
}

void UnLoadGameCode(GameCode *gameCode)
{
    if(gameCode->gameCodeDLL)
    {
        FreeLibrary(gameCode->gameCodeDLL);
        gameCode->gameCodeDLL = NULL;
    }
    
    gameCode->GameUpdateAndRenderP = GameUpdateAndRenderStub;
    gameCode->LoadDataP = LoadDataStub;
    gameCode->HandleEventP = HandleEventStub;
    gameCode->ShowMainMenuP = ShowMainMenuStub;
    gameCode->ShowCreditsP = ShowCreditsStub;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize TTF : %s\n", TTF_GetError());
    }
    
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());
    }
    
    window = SDL_CreateWindow("GameZero",
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  SCREEN_WIDTH,
							  SCREEN_HEIGHT,
							  0);
    
    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, 3, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
    
    GameCode gameCode = LoadGameCode();
    
    while(!quitGlobal)
    {
        switch(gameMode)
        {
            case GAME_MAIN_MENU:
            gameCode.ShowMainMenuP(&gameMode, &event, renderer);
            break;
            
            case GAME_PLAY:
            f32 deltaTime = 0.016f;
            u64 startTime = 0;
            u64 endTime = 0;
            
            gameCode.LoadDataP(renderer);
            
            bool quitGame = false;
            
            while(!quitGame)
            {
                startTime = SDL_GetPerformanceCounter();
                
                if(SDL_PollEvent(&event))
                {
                    switch(event.type)
                    {
                        case SDL_QUIT:
                        quitGame = true;
                        gameMode = GAME_EXIT;
                        break;
                        
                        case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quitGame = true;
                            gameMode = GAME_EXIT;
                        }
                        else if(event.key.keysym.sym == SDLK_r) // reloading game data
                        {
                            gameCode.LoadDataP(renderer);
                        }
                        break;
                    }
                    gameCode.HandleEventP(&event);
                }
                
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
                SDL_RenderClear(renderer);
                
                gameCode.GameUpdateAndRenderP(deltaTime, renderer);
                
                SDL_RenderPresent(renderer);
                
                endTime = SDL_GetPerformanceCounter();
                u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
                deltaTime = (float)elapsed / 1000.0f;
                
                if(deltaTime > 0.016f)
                {
                    deltaTime = 0.016f;
                }
            }
            
            break;
            
            case GAME_CREDITS:
            gameCode.ShowCreditsP(&gameMode, &event, renderer);
            break;
            
            case GAME_EXIT:
            quitGlobal = true;
            break;
            
            default:
            quitGlobal = true;
        }
    }
    
    UnLoadGameCode(&gameCode);
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}