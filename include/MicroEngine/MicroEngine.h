//Micro Game Engine
/*
Author :: Abhijith.C.V (Lone-coder)

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

#include "ME_GameObject.h"
#include "ME_Vector2D.h"

int ME_Init(const char *title, int screenWidth, int screenHeight);

void ME_Run(void (*handleEvents)(SDL_Event event),
            void (*update)(float deltaTime),
            void (*render)(SDL_Renderer *renderer));

void ME_Quit();

SDL_Window* ME_GetWindow();

SDL_Renderer* ME_GetRenderer();

int ME_GetScreenWidth();

int ME_GetScreenHeight();

#endif //GAMEENGINE_H
