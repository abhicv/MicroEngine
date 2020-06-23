#include "MicroEngine.h"

#include "ME_GameObject.c"
#include "ME_ParticleSystem.c"
#include "ME_Utility.c"
#include "ME_Vector2D.c"
#include "MicroUI.c"
#include "types.h"

internal Game ME_CreateGame(const char *gameTitle, i32 gameWidth, i32 gameHeight)
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

    platform.window =
        SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, gameWidth, gameHeight, 0);

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

    SDL_Log("MicroEngine => Window dimensions -> %d x %d", gameWidth, gameHeight);
    SDL_Log("MicroEngine => Renderer -> %s", rendererInfo.name);

    Game game = {0};
    game.gameWidth = gameWidth;
    game.gameHeight = gameHeight;
    game.platform = platform;
    game.isRunning = true;

    return game;
}

internal int ME_Init(const char *title, u32 screenWidth, u32 screenHeight)
{
    width = screenWidth;
    height = screenHeight;

    // SDL init
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() != 0)
        SDL_Log("Failed to initialize TTF : %s\n", SDL_GetError());

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, 0);

    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 0;
    }

    u32 rendflag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, rendflag);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);

    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 0;
    }

    ME_SetRenderColor(renderer, defaultDrawColor);

    SDL_Log("MicroEngine :: Renderer : %s", rendererInfo.name);
    SDL_Log("MicroEngine :: Window dimensions : %d x %d", width, height);

    return 1;
}

internal void ME_RunGame(Game *game)
{
    // Spalsh screen
    SDL_Renderer *renderer = game->platform.renderer;
    f32 deltaTime = 0.0f;

    // SDL_Texture *splashTextre = IMG_LoadTexture(renderer, "assets/MicroEngine_SplashScreen_2.png");
    // SDL_Rect splashRect = {0};

    // SDL_QueryTexture(splashTextre, 0, 0, &splashRect.w, &splashRect.h);

    // splashRect.w /= 5;
    // splashRect.h /= 5;

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    // SDL_RenderClear(renderer);
    // SDL_RenderCopy(renderer, splashTextre, NULL, &splashRect);
    // SDL_RenderPresent(renderer);

    MUI_TextBox fpsText = MUI_CreateTextBox(30, 10, 25);
    MUI_SetTextBoxColor(&fpsText, ME_HexToSdlColor(0xffff00));

    // SDL_Delay(2000);

    // main loop
    while (game->isRunning)
    {
        startTime = SDL_GetPerformanceCounter();

        // Event handling
        while (SDL_PollEvent(&game->platform.event))
        {
            switch (game->platform.event.type)
            {
            case SDL_QUIT:
                game->isRunning = false;
                break;
            }

            if(game->handleEvent != NULL)
                game->handleEvent(game->platform.event);
        }

        // Update
        if(game->update != NULL)
            game->update(game->deltaTime);

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0,0,0,0);
        SDL_RenderClear(renderer);

        if(game->render != NULL)
            game->render(renderer);

        MUI_RenderTextBox(&fpsText, renderer, MUI_TEXT_SOLID);
        SDL_RenderPresent(renderer);

        // Calculating delta time
        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;
        game->deltaTime = deltaTime;

        sprintf(fpsText.textString, "%0.2f", 1 / deltaTime);

        // printf("delta : %f\n", deltaTime);
        // deltaTime = deltaTime < 0.016f ? 0.016f : deltaTime;
    }
}

internal void ME_Run(HandleEventCall handleEvent, UpdateCall update, RenderCall render)
{
    // Spalsh screen
    SDL_Texture *splashTextre = IMG_LoadTexture(renderer, "assets/MicroEngine_SplashScreen_2.png");
    SDL_Rect splashRect = {0};

    SDL_QueryTexture(splashTextre, 0, 0, &splashRect.w, &splashRect.h);

    splashRect.w /= 5;
    splashRect.h /= 5;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, splashTextre, NULL, &splashRect);
    SDL_RenderPresent(renderer);

    MUI_TextBox fpsText = MUI_CreateTextBox(30, 10, 25);
    MUI_SetTextBoxColor(&fpsText, ME_HexToSdlColor(0xffff00));

    SDL_Delay(2000);

    //  loop
    while (!quit)
    {
        startTime = SDL_GetPerformanceCounter();
        
        // Event handling
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            }
            handleEvent(event);
        }

        // Update
        update(deltaTime);

        // Rendering
        SDL_RenderClear(renderer);
        render(renderer);
        MUI_RenderTextBox(&fpsText, renderer, MUI_TEXT_SOLID);
        SDL_RenderPresent(renderer);

        // Calculating delta time
        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;

        sprintf(fpsText.textString, "%0.2f", 1 / deltaTime);

        // printf("delta : %f\n", deltaTime);
        // deltaTime = deltaTime < 0.016f ? 0.016f : deltaTime;
    }
    // cleaning fps text box
    // MUI_DestroyTextBox(&fpsText);
}

internal void ME_QuitGame(Game *game)
{
    SDL_DestroyRenderer(game->platform.renderer);
    SDL_DestroyWindow(game->platform.window);

    game->platform.renderer = NULL;
    game->platform.window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

internal void ME_Quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    renderer = NULL;
    window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

internal SDL_Window *ME_GetWindow() { return window; }
internal SDL_Renderer *ME_GetRenderer() { return renderer; }
internal int ME_GetScreenWidth() { return width; }
internal int ME_GetScreenHeight() { return height; }
