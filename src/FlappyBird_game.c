#include "../include/MicroEngine/MicroEngine.h"
#include "../include/MicroEngine/ME_Utility.h"

#define foreach(count) int i = 0; \
                        for(i = 0; i < count; i++)

#define obstacle_max_count 4
#define distance_btw_obstacles 300

ME_GameObject *player;
ME_GameObject *obstacles[obstacle_max_count];

Vector2 velocity = {0, 0};
float gravity = 200.0f;

void HandleEvents(SDL_Event event)
{
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = -300.0f;
    }

    if(event.type == SDL_KEYUP)
    {
        if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = 200.0f;
    }
}

void Update(float deltaTime)
{
    velocity.y += gravity * deltaTime;
    player->position.y += velocity.y * deltaTime;

    if(player->position.y > ME_GetScreenHeight())
    {
        velocity.y = 0;
    }

    foreach(obstacle_max_count)
    {
        obstacles[i]->position.x += -150 * deltaTime;

        //reseting position of obstacles
        if(obstacles[i]->position.x <= 0 - 50)
        {
             obstacles[i]->position.y = ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight());

             if(i != 0)
                obstacles[i]->position.x = obstacles[i - 1]->position.x + distance_btw_obstacles;

             if(i == 0)
                obstacles[0]->position.x = obstacles[obstacle_max_count - 1]->position.x + distance_btw_obstacles;
        }

    }


}

void Render(SDL_Renderer *renderer)
{
    ME_RenderGameObject(player, renderer);

    int y;

    foreach(obstacle_max_count)
    {
        ME_RenderGameObject(obstacles[i], renderer);
        y = obstacles[i]->position.y;

        obstacles[i]->position.y -= 550;
        obstacles[i]->angle = 180.0f;
        ME_RenderGameObject(obstacles[i], renderer);
        obstacles[i]->angle = 0;
        obstacles[i]->position.y = y;
    }
}

int main(int argc, char *argv[])
{
    //MicroEngine initialization
    if(!ME_Init("Flappy Bird Clone", 420, 640))
    {
        ME_Quit();
        return 1;
    }

    //Game objects Initialization
    player = ME_CreateGameObject(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    player->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Player.png");

    foreach(obstacle_max_count)
    {
        obstacles[i] = ME_CreateGameObject(ME_GetScreenWidth() + 100, ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight()));
        obstacles[i]->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/obstacle.png");
        obstacles[i]->destRect.w = 100;
        obstacles[i]->destRect.h = 400;

        if(i != 0)
            obstacles[i]->position.x = obstacles[i -1]->position.x + distance_btw_obstacles;
    }

    //Game loop
    ME_Run(&HandleEvents, &Update, &Render);

    //Cleaning up everything
    ME_DestroyGameObject(player);

    for(i = 0 ; i < obstacle_max_count; i++)
    {
        ME_DestroyGameObject(obstacles[i]);
    }

    ME_Quit();

    return 0;
}
