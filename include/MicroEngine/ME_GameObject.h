#ifndef ME_GAMEOBJECT_H
#define ME_GAMEOBJECT_H

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ME_Vector2D.h"

typedef struct {
    Vector2 position;
    float angle;
    Vector2 scale;
    bool enabled;

    int nFrames;
    bool animate;

    SDL_Texture *texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Color debugColor;

} ME_GameObject;

ME_GameObject* ME_CreateGameObject(int, int);
void ME_UpdateGameObject(ME_GameObject *gameObject);
void ME_RenderGameObject(ME_GameObject *gameObject, SDL_Renderer*);
void ME_DestroyGameObject(ME_GameObject *gameObject);

#endif //ME_GAMEOBJECT_H
