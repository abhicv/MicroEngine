#include "../../include/MicroEngine/ME_GameObject.h"
#include "../../include/MicroEngine/ME_Utility.h"

ME_GameObject* ME_CreateGameObject(int xPos, int yPos)
{
    ME_GameObject *gameObject = (ME_GameObject*)malloc(sizeof(ME_GameObject));

    gameObject->enabled = true;
    gameObject->animate = false;
    gameObject->position = NewVector2(xPos,yPos);
    gameObject->nFrames = 0;
    gameObject->texture = NULL;

    gameObject->angle = 0;

    gameObject->destRect.w = 32;
    gameObject->destRect.h = 32;

    gameObject->srcRect.x = 0;
    gameObject->srcRect.y = 0;
    gameObject->srcRect.w = 32;
    gameObject->srcRect.h = 32;


    SDL_Color tmpColor = { rand() % 256, rand() % 256, rand() % 256, 255 };
    gameObject->debugColor = tmpColor;

    return gameObject;
}

void ME_UpdateGameObject(ME_GameObject *gameObject)
{
    if(gameObject->animate && gameObject->nFrames > 0)
        gameObject->srcRect.x = gameObject->srcRect.w * (((int)SDL_GetTicks() / 60) % gameObject->nFrames);
    else
        gameObject->srcRect.x = 0;
}

void ME_RenderGameObject(ME_GameObject *gameObject, SDL_Renderer *renderer)
{
    gameObject->destRect.x = (int)(gameObject->position.x - gameObject->destRect.w / 2);
    gameObject->destRect.y = (int)(gameObject->position.y - gameObject->destRect.h / 2);

    SDL_Rect debugRect = gameObject->destRect;
    debugRect.x = gameObject->position.x;
    debugRect.y = gameObject->position.y;


    if(gameObject->enabled)
    {
        if(gameObject->texture != NULL)
        {
            SDL_QueryTexture(gameObject->texture, NULL, NULL, &gameObject->srcRect.w, &gameObject->srcRect.h);
            SDL_RenderCopyEx(renderer,gameObject->texture,&gameObject->srcRect,&gameObject->destRect,gameObject->angle,NULL,SDL_FLIP_NONE);
        }
        else
        {
            ME_RenderFillRect(renderer, &gameObject->destRect, gameObject->debugColor);
        }
    }
}

void ME_DestroyGameObject(ME_GameObject *gameObject)
{
    SDL_DestroyTexture(gameObject->texture);
    free(gameObject);
    gameObject = NULL;
}
