#ifndef ME_RENDERER_H
#define ME_RENDERER_H

#include "types.h"

#include <SDL2/SDL.h>

//NOTE(Lonecoder) : For SDL2 hardware acceleration rendering
typedef struct
{
    SDL_Texture *texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_RendererFlip flipFlag;
    SDL_Point rotationCenter;
    f32 angle;
    SDL_Renderer *targetRenderer;

} ME_RenderData;

void ME_CreateRenderData(SDL_Texture *texture,
                         SDL_Rect srcRect,
                         SDL_Rect destRect,
                         SDL_RendererFlip flipFlag,
                         SDL_Point rotationCenter, f32 angle);

void Render(ME_RenderData *renderData);

#endif // ME_RENDERER_H
