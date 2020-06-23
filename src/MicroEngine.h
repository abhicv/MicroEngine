/*
MICRO ENGINE
AUTHOR : Abhijith C V(Lone-coder)
*/
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

typedef void (*HandleEventCall)(SDL_Event event);
typedef void (*UpdateCall)(float deltaTime);
typedef void (*RenderCall)(SDL_Renderer *renderer);

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

} SDLPlatform;

typedef struct Input;

typedef struct 
{
    SDLPlatform platform;
    
    bool isRunning;
    f32 deltaTime;
    u32 gameWidth;
    u32 gameHeight;
    char *gameTitle;

    HandleEventCall handleEvent;
    UpdateCall update;
    RenderCall render; 

} Game;

//globals
//TODO(Lone-coder) : remove this globals
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
u32 width = 0;
u32 height = 0;

bool quit = false;
u64 startTime = 0;
u64 endTime = 0;
f32 deltaTime = 0.016f;
SDL_Event event;

SDL_Color defaultDrawColor = {0, 0, 0, 0};

internal Game ME_CreateGame(const char *gameTitle, i32 gameWidth, i32 gameHeight);
internal void ME_RunGame(Game *game); 

internal int ME_Init(const char *title, u32 screenWidth, u32 screenHeight);
internal void ME_Run(HandleEventCall handleEvent, UpdateCall update, RenderCall render);
internal void ME_Quit();

SDL_Window *ME_GetWindow();
SDL_Renderer *ME_GetRenderer();
int ME_GetScreenWidth();
int ME_GetScreenHeight();
#endif //GAMEENGINE_H
