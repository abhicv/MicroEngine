#ifndef MICROENGINE_H
#define MICROENGINE_H

//Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

//SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//MicroEngine headers
#include "types.h"

#define MAX(A, B) A < B ? B : A
#define MIN(A, B) A > B ? B : A 

#define loop(i, count) for(i = 0; i < count; i++)

typedef void (*HandleEventCall)(SDL_Event event);
typedef void (*UpdateCall)(f32 deltaTime);
typedef void (*RenderCall)(SDL_Renderer *renderer);

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

} SDLPlatform;

typedef struct
{
    i32 mouseX;
    i32 mouseY;
    bool leftMouseButtonDown;
    bool rightMouseButtonDown;

} MouseInputState;

typedef struct
{
    SDLPlatform platform;

    bool isRunning;
    f32 deltaTime;
    u32 windowWidth;
    u32 windowHeight;
    char *gameTitle;

    //NOTE(Loncoder): function pointers provided by the user
    HandleEventCall handleEvent;
    UpdateCall update;
    RenderCall render;

} ME_Game;

ME_Game ME_CreateGame(char *gameTitle, u32 windowWidth, u32 windowHeight);
void ME_RunGame(ME_Game *game, bool showSplashScreen);
void ME_QuitGame(ME_Game *game);

#endif //GAMEENGINE_H
