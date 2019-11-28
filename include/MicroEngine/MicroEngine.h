//Micro Game Engine

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

int ME_Init(const char*, int, int);
void ME_Quit();

SDL_Renderer* ME_GetRenderer();
int ME_GetScreenWidth();
int ME_GetScreenHeight();
void ME_GetDeltaTime(float*);

#endif //GAMEENGINE_H
