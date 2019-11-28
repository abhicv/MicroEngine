#ifndef MICROPHYSICS_H
#define MICROPHYSICS_H

#include <SDL2/SDL.h>

#include "../MicroEngine/ME_Vector2D.h"

typedef struct
{
    int objectID;
    SDL_Rect collisionRect;
    float mass;
    Vector2 velocity;

}MP_DynamicObject;

MP_DynamicObject MP_CreateDynamic(float ,SDL_Rect);

typedef struct
{
    int objectID;
    SDL_Rect collisionRect;

}MP_StaticObject;


#endif // MICROPHYSICS_H
