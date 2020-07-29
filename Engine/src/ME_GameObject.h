#ifndef ME_GAMEOBJECT_H
#define ME_GAMEOBJECT_H

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "ME_Vector2D.h"

#define TIME_BTW_FRAMES 100
typedef struct
{
    Vector2 position;
    f32 angle;
    Vector2 scale;
    bool enabled;

    u32 nFrames;
    bool animate;

    SDL_Texture *texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Color debugColor;

} ME_GameObject;

ME_GameObject *ME_CreateGameObject(i32, i32);
void ME_UpdateGameObject(ME_GameObject *gameObject);
void ME_RenderGameObject(ME_GameObject *gameObject, SDL_Renderer *);
void ME_DestroyGameObject(ME_GameObject *gameObject);

#endif //ME_GAMEOBJECT_H
