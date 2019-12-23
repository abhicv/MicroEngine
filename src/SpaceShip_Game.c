#include "../include/MicroEngine/MicroEngine.h"
#include "../include/MicroEngine/ME_Utility.h"

ME_GameObject *spaceship;
SDL_Point mousePos;
ME_GameObject *bullets[50];
float velocity[50];
Vector2 bulletDir[50];
int currentBulletIndex;

void Shoot()
{
    velocity[currentBulletIndex] = 500.0f;

    bulletDir[currentBulletIndex].x = mousePos.x - spaceship->position.x;
    bulletDir[currentBulletIndex].y = mousePos.y - spaceship->position.y;

    Vector2Normalize(&bulletDir[currentBulletIndex]);

    currentBulletIndex++;

    int i = 0;

    if(currentBulletIndex >= 50)
    {
        currentBulletIndex = 0;

        for(i = 0; i < 50; i++)
        {
            bullets[i]->position = spaceship->position;
            velocity[i] = 0;
            //bulletDir[i] = NewVector2(0, 0);
        }
    }

}

void HandleEvents(SDL_Event event)
{
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    DEBUG_LOG_COORDINATES(mousePos);

    spaceship->angle = - 180 / M_PI * atan2(mousePos.x - spaceship->position.x, mousePos.y - spaceship->position.y);

    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        Shoot();
    }
}


void Update(float deltaTime)
{
    int i = 0;

    for(i = 0; i < 50; i++)
    {
        bullets[i]->position.x += velocity[i] * bulletDir[i].x * deltaTime;
        bullets[i]->position.y += velocity[i] * bulletDir[i].y * deltaTime;
    }

}

void Render(SDL_Renderer *renderer)
{
    SDL_Color rendColor = ME_GetRenderColor(renderer);

    ME_RenderGameObject(spaceship, renderer);

    ME_SetRenderColor(renderer, ME_HexToSdlColor(0xff00ff));
    SDL_RenderDrawLine(renderer, spaceship->position.x, spaceship->position.y, mousePos.x, mousePos.y);
    ME_SetRenderColor(renderer, rendColor);

    int i = 0;

    for(i = 0; i < 50; i++)
    {
        ME_RenderGameObject(bullets[i], renderer);
    }
}

int main(int argc, char *argv[])
{
    if(!ME_Init("SpaceShip Game", 1024, 720))
    {
        ME_Quit();
        return 1;
    }


    //Initialization
    int i = 0;

    spaceship = ME_CreateGameObject(400, 300);
    spaceship->texture = IMG_LoadTexture(ME_GetRenderer(),"assets/Sprites/spaceship.png");
    spaceship->destRect.w = 128;
    spaceship->destRect.h = 128;

    for(i = 0; i < 50; i++)
    {
        bullets[i] = ME_CreateGameObject(400, 300);
        bullets[i]->destRect.w = 15;
        bullets[i]->destRect.h = 15;

        velocity[i] = 0;
        bulletDir[i] = NewVector2(0, 0);
    }

    ME_Run(&HandleEvents, &Update, &Render);

    //Cleaning up everything
    ME_DestroyGameObject(spaceship);
    for(i = 0; i < 50; i++)
    {
        ME_DestroyGameObject(bullets[i]);
    }

    ME_Quit();

    return 0;
}
