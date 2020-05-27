#ifndef ME_RENDERER_H
#define ME_RENDERER_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture *texture;
    SDL_Rect *srcRect;
    SDL_Rect *destRect;
    SDL_RendererFlip flipFlag;
    SDL_Point *rotationCenter;
    double angle;

} ME_RenderRequest;

void ME_CreateRenderRequest(SDL_Texture *,
                            SDL_Rect *,
                            SDL_Rect *,
                            SDL_RendererFlip,
                            SDL_Point *, double);

void ME_HandleRenderRequest();

#endif // ME_RENDERER_H
