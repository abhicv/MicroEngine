#include "../../include/MicroEngine/ME_GameObject.h"

ME_GameObject* ME_CreateGameObject(int xPos, int yPos)
{
    ME_GameObject *gameObject = (ME_GameObject*)malloc(sizeof(ME_GameObject));

    gameObject->enabled = true;
    gameObject->animate = false;
    gameObject->position = NewVector2(xPos,yPos);
    gameObject->nFrames = 0;
    gameObject->texture = NULL;

    gameObject->destRect.w = 64;
    gameObject->destRect.h = 64;

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
    gameObject->destRect.x = (int)(gameObject->position.x - gameObject->destRect.w / 2);
    gameObject->destRect.y = (int)(gameObject->position.y - gameObject->destRect.h / 2);

    if(gameObject->animate && gameObject->nFrames > 0)
        gameObject->srcRect.x = gameObject->srcRect.w * (((int)SDL_GetTicks() / 60) % gameObject->nFrames);
    else
        gameObject->srcRect.x = 0;
}

void ME_RenderGameObject(ME_GameObject *gameObject, SDL_Renderer *rend)
{
    if(gameObject->enabled)
    {
        if(gameObject->texture != NULL)
        {
            SDL_RenderCopyEx(rend,gameObject->texture,&gameObject->srcRect,&gameObject->destRect,0,NULL,SDL_FLIP_HORIZONTAL);
        }
        else
        {
            SDL_SetRenderDrawColor(rend, gameObject->debugColor.r, gameObject->debugColor.g, gameObject->debugColor.b, 255);
            SDL_RenderFillRect(rend, &gameObject->destRect);
            SDL_SetRenderDrawColor(rend, 0,0,0,255);
        }
    }
}

void ME_DestroyGameObject(ME_GameObject *gameObject)
{
    SDL_DestroyTexture(gameObject->texture);

    free(gameObject);

    gameObject = NULL;
}
