#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <windows.h>

#include "sdl2_gamezero.h"

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
    
    while(!quitGlobal)
    {
        switch (gameMode)
        {
            case GAME_MAIN_MENU:
            ShowMainMenu(&gameMode, &event, renderer);
            break;
            
            case GAME_PLAY:
            f32 deltaTime = 0.016f;
            u64 startTime = 0;
            u64 endTime = 0;
            u64 freq = SDL_GetPerformanceFrequency();
            
            bool quitGame = false;
            
            while(!quitGame)
            {
                startTime = SDL_GetPerformanceCounter();
                
                while(SDL_PollEvent(&event) != 0)
                {
                    switch(event.type)
                    {
                        case SDL_QUIT:
                        quitGame = true;
                        gameMode = GAME_EXIT;
                        break;
                        
                        case SDL_KEYDOWN:
                        if(event.key.keysym.sym == SDLK_r) // loading next level
                        {
                            quitGame = true;
                            gameMode = GAME_LOAD_LEVEL;
                        }
                        else if(event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quitGame = true;
                            gameMode = GAME_LEVEL_MENU;
                        }
                        break;
                    }
                    HandleEvent(&event);
                }
                
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
                SDL_RenderClear(renderer);
                GameUpdateAndRender(&gameMode, &quitGame, deltaTime, renderer);
                SDL_RenderPresent(renderer);
                
                endTime = SDL_GetPerformanceCounter();
                u64 elapsed = (endTime - startTime) * 1000 / freq;
                deltaTime = (float)elapsed / 1000.0f;
                
                if(deltaTime > 0.016f)
                {
                    deltaTime = 0.016f;
                }
            }
            
            break;
            
            case GAME_LOAD_LEVEL:
            LoadData(renderer);
            gameMode = GAME_PLAY;
            break;
            
            case GAME_LEVEL_MENU:
            ShowLevelMenu(&gameMode, &event, renderer);
            break;
            
            case GAME_CREDITS:
            ShowCredits(&gameMode, &event, renderer);
            break;
            
            case GAME_EXIT:
            quitGlobal = true;
            break;
            
            default:
            quitGlobal = true;
            break;
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
} 