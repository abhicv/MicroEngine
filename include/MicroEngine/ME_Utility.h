#ifndef ME_UTILITY_H
#define ME_UTILITY_H

#include <SDL2\SDL.h>
#include <stdio.h>

#define PRINT_COLOR(Color) printf("r: %d, g: %d, b: %d\n",Color.r,Color.g,Color.b)

typedef unsigned int ME_Color;

SDL_Color ME_HexToSdlColor(ME_Color);
SDL_Color ME_RandomSdlColor();
void ME_SetRenderColor(SDL_Renderer*, SDL_Color);
void ME_RenderDrawRect(SDL_Renderer*, const SDL_Rect*, SDL_Color);
void ME_RenderFillRect(SDL_Renderer*, const SDL_Rect*, SDL_Color);


#endif // ME_UTILITY_H
