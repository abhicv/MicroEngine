//Micro Engine core
#include "../../include/MicroEngine/MicroEngine.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int width = 0;
static int height = 0;

const SDL_Color dfDrawColor = {0,0,0,255};

int ME_Init(const char *title, int screenWidth, int screenHeight)
{
    width = screenWidth;
    height = screenHeight;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initiate SDL : %s\n",SDL_GetError());
        return 0;
    }

    if(TTF_Init() != 0)
        SDL_Log("Failed to initiate TTF : %s\n",SDL_GetError());

    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
        SDL_Log("Failed to initiate IMG : %s\n",IMG_GetError());

    window = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,0);

    if(window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n",SDL_GetError());
        return 0;
    }

    Uint32 rendflag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window,3,rendflag);

    if(renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n",SDL_GetError());
        return 0;
    }

    SDL_SetRenderDrawColor(renderer,dfDrawColor.r,dfDrawColor.g,dfDrawColor.b,dfDrawColor.a);

    return 1;
}

void ME_Quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    renderer = NULL;
    window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Window* ME_GetWindow()
{
    return window;
}

SDL_Renderer* ME_GetRenderer()
{
    return renderer;
}

int ME_GetScreenWidth()
{
    return width;
}

int ME_GetScreenHeight()
{
    return height;
}

void ME_GetDeltaTime(float *deltaTime)
{
    static Uint64 currentTime = 0;
    static Uint64 previousTime = 0;

    if(previousTime == 0 && currentTime == 0)
        return;

    previousTime = currentTime;
    currentTime = SDL_GetPerformanceCounter();

    *deltaTime = (float)((currentTime - previousTime) * 1000/ SDL_GetPerformanceFrequency());
    *deltaTime *= 0.001f;
}
