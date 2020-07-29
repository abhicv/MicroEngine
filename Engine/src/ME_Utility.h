#ifndef ME_UTILITY_H
#define ME_UTILITY_H

#include <SDL2\SDL.h>
#include <stdio.h>

typedef unsigned int ME_Color;

void PrintBinary(unsigned int integer);

float MapFloat(float x, float in_min, float in_max, float out_min, float out_max);

long MapInt(long x, long in_min, long in_max, long out_min, long out_max);

SDL_Color ME_HexToSdlColor(ME_Color);

SDL_Color ME_RandomSdlColor();

int ME_Random(int min, int max);

void ME_SetRenderColor(SDL_Renderer *, SDL_Color);

SDL_Color ME_GetRenderColor(SDL_Renderer *);

void ME_RenderDrawRect(SDL_Renderer *, const SDL_Rect *, SDL_Color);

void ME_RenderFillRect(SDL_Renderer *, const SDL_Rect *, SDL_Color);

#endif // ME_UTILITY_H
