//Micro Engine core components
#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroEngine/ME_Utility.h"
#include "../../include/MicroUI/MicroUI.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static int width = 0;
static int height = 0;

bool quit = false;
int startTime = 0;
int endTime = 0;
float deltaTime = 0.016f;
SDL_Event event;

float pauseFactor = 1;

const SDL_Color dfDrawColor = {0,0,0,255};

void Pause()
{
    pauseFactor = 0.0f;
}

void UnPause()
{
    pauseFactor = 1.0f;
}

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

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    if(window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n",SDL_GetError());
        return 0;
    }

    Uint32 rendflag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, rendflag);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);


    if(renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n",SDL_GetError());
        return 0;
    }

    ME_SetRenderColor(renderer, dfDrawColor);

    SDL_Log("MicroEngine :: Renderer : %s",rendererInfo.name);
    SDL_Log("MicroEngine :: Window dimensions : %d x %d",width, height);

    return 1;
}

void ME_Run(void (*HandleEvents)(SDL_Event event),
            void (*Update)(float deltaTime),
            void (*Render)(SDL_Renderer *renderer))
{
    //Spalsh screen
    SDL_Texture *splashTextre = IMG_LoadTexture(renderer, "assets/MicroEngine_SplashScreen_2.png");
    SDL_Rect splashRect = {0,0, width, height};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, splashTextre, NULL, &splashRect);
    SDL_RenderPresent(renderer);

    SDL_Delay(1000);

    MUI_TextBox fpsText = MUI_CreateTextBox(30, 10, 25);
    MUI_SetTextBoxColor(&fpsText, ME_HexToSdlColor(0xffff00));


    while(!quit)
    {
        startTime = SDL_GetPerformanceCounter();

        //Event handling
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            }

            HandleEvents(event);
        }

        //Update
        Update(deltaTime);

        //Rendering
        SDL_RenderClear(renderer);
        Render(renderer);
        MUI_RenderTextBox(&fpsText, renderer, MUI_TEXT_SOLID);
        SDL_RenderPresent(renderer);

        //Calculating delta time
        endTime = SDL_GetPerformanceCounter();
        deltaTime = (float)((endTime - startTime) * 1000 / SDL_GetPerformanceFrequency());
        deltaTime *= 0.001f;
        deltaTime *= pauseFactor;

        sprintf(fpsText.textString, "%0.2f", 1/deltaTime);
        //deltaTime = deltaTime < 0.016f ? 0.016f : deltaTime;
    }

    //cleaning fps text box
    MUI_DestroyTextBox(&fpsText);
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

