#include "../../include/MicroEngine/ME_Utility.h"

SDL_Color ME_HexToSdlColor(ME_Color color)
{
    SDL_Color sdlColor;

    sdlColor.r = (color >> 16) & 0xFF;
    sdlColor.g = (color >> 8) & 0xFF;
    sdlColor.b = color & 0xFF;
    sdlColor.a = 255;

    return sdlColor;
}

SDL_Color ME_RandomSdlColor()
{
    SDL_Color sdlColor = { rand() % 256, rand() % 256, rand() % 256, 255 };
    return sdlColor;
}

void ME_SetRenderColor(SDL_Renderer *renderer, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

SDL_Color ME_GetRenderColor(SDL_Renderer *renderer)
{
    SDL_Color rendColor;

    SDL_GetRenderDrawColor(renderer, &rendColor.r, &rendColor.g, &rendColor.b, &rendColor.a);

    return rendColor;
}

void ME_RenderDrawRect(SDL_Renderer *renderer, const SDL_Rect *rect, SDL_Color color)
{
    SDL_Color rendColor;
    SDL_GetRenderDrawColor(renderer, &rendColor.r,&rendColor.g,&rendColor.b,&rendColor.a);

    ME_SetRenderColor(renderer, color);
    SDL_RenderDrawRect(renderer, rect);

    ME_SetRenderColor(renderer, rendColor);

}

void ME_RenderFillRect(SDL_Renderer *renderer, const SDL_Rect *rect, SDL_Color color)
{
    SDL_Color rendColor;
    SDL_GetRenderDrawColor(renderer, &rendColor.r,&rendColor.g,&rendColor.b,&rendColor.a);

    ME_SetRenderColor(renderer, color);
    SDL_RenderFillRect(renderer, rect);

    ME_SetRenderColor(renderer, rendColor);

}

