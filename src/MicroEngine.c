#include "MicroEngine.h"

#include "ME_GameObject.c"
#include "ME_ParticleSystem.c"
#include "ME_Utility.c"
#include "ME_Vector2D.c"
#include "MicroUI.c"

#include "ME_debug.h"
#include "types.h"

//NOTE(LoneCoder): For debug of performance
global MUI uiPerf = {.fontFile = OPENSANS_FONT_FILE};
global char fpsString[25] = "No text";

internal ME_Game ME_CreateGame(char *gameTitle, i32 windowWidth, i32 windowHeight)
{
    SDLPlatform platform = {0};

    // SDL initilization
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
    }

    // SDL ttf font
    if (TTF_Init() != 0)
        SDL_Log("Failed to initialize TTF : %s\n", SDL_GetError());

    // SDL image
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());

    platform.window = SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);

    if (platform.window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
    }

    u32 rendflag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    platform.renderer = SDL_CreateRenderer(platform.window, -1, rendflag);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(platform.renderer, &rendererInfo);

    if (platform.renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
    }

    printf("MicroEngine => Window dimensions -> %d x %d\n", windowWidth, windowHeight);
    printf("MicroEngine => Renderer -> %s\n", rendererInfo.name);

    ME_Game game = {0};
    game.gameTitle = gameTitle;
    game.windowWidth = windowWidth;
    game.windowHeight = windowHeight;
    game.platform = platform;
    game.isRunning = true;
    game.deltaTime = 0.016f;
    game.handleEvent = NULL;
    game.update = NULL;
    game.render = NULL;

    return game;
}

internal void ME_RunGame(ME_Game *game)
{
    // Spalsh screen
    SDL_Renderer *renderer = game->platform.renderer;
    f32 deltaTime = 0.016f;
    u64 startTime = 0;
    u64 endTime = 0;

    SDL_Texture *splashTextre = IMG_LoadTexture(renderer, "assets/MicroEngine_SplashScreen_2.png");
    SDL_Rect splashRect = {0};

    SDL_QueryTexture(splashTextre, 0, 0, &splashRect.w, &splashRect.h);

    splashRect.w /= 5;
    splashRect.h /= 5;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // SDL_RenderCopy(renderer, splashTextre, NULL, &splashRect);
    // SDL_RenderPresent(renderer);
    // SDL_Delay(2000);

    MUI_Rect rect = {60, 15, 120, 30};

    // main loop
    while (game->isRunning)
    {
        //for debug ui
        MUI_BeginFrame(&uiPerf, NULL);
        MUI_TextP(&uiPerf, MUI_IdInit(__LINE__, 100), rect, fpsString, 20);

        startTime = SDL_GetPerformanceCounter();

        // Event handling
        while (SDL_PollEvent(&game->platform.event))
        {
            switch (game->platform.event.type)
            {
            case SDL_QUIT:
                game->isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (game->platform.event.key.keysym.sym == SDL_SCANCODE_ESCAPE)
                    game->isRunning = false;
                break;
            }

            if (game->handleEvent != NULL)
                game->handleEvent(game->platform.event);
        }

        // Update
        if (game->update != NULL)
            game->update(game->deltaTime);

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        if (game->render != NULL)
            game->render(renderer);

        MUI_EndFrame(&uiPerf, renderer);
        SDL_RenderPresent(renderer);

        // Calculating delta time
        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;
        game->deltaTime = deltaTime;

        sprintf(fpsString, "%0.2f FPS", 1 / deltaTime);

        // printf("delta : %f\n", deltaTime);
        // deltaTime = deltaTime < 0.016f ? 0.016f : deltaTime;
    }
}

internal void ME_QuitGame(ME_Game *game)
{
    SDL_DestroyRenderer(game->platform.renderer);
    SDL_DestroyWindow(game->platform.window);

    game->platform.renderer = NULL;
    game->platform.window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}