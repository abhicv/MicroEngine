#include "../src/MicroEngine.c"

//NOTE(loneCoder) : assets from -> https://ansimuz.itch.io/spaceship-shooter-environment
#define MUI_ORIGIN_ID 3020

global ME_Game game = {0};
// global MUI ui = {.fontFile = VCR_OSD_MONO_FONT_FILE};
global MUI ui = {.fontFile = BIT_5x3_FONT_FILE};
// global MUI ui = {.fontFile = "assets/Font/baby_blocks.ttf"};
// global MUI ui = {.fontFile = "assets/Font/Fipps-REgular.otf"};
// global MUI ui = {.fontFile = "assets/Font/Top_Secret.ttf"};

global ME_GameObject *spaceship = NULL;
global ME_GameObject *enemySmall = NULL;
global ME_GameObject *bullets[50];
global ME_GameObject *explosion = NULL;

global SDL_Texture *background = NULL;
global Vector2 spaceShipVel = {0.0f, 0.0f};
global SDL_Point mousePos = {0};
global f32 velocity[50];
global Vector2 bulletDir[50] = {0.0f, 0.0f};
global u32 currentBulletIndex = 0;

global u32 score = 0;
global char scoreText[10];
global u32 timer =  0;
bool enableTimer = false;

void Shoot()
{
    bullets[currentBulletIndex]->enabled = true;
    bullets[currentBulletIndex]->position = spaceship->position;
    velocity[currentBulletIndex] = 1000.0f;

    bulletDir[currentBulletIndex].x = 0.0f;
    bulletDir[currentBulletIndex].y = -1.0f;

    Vector2Normalize(&bulletDir[currentBulletIndex]);

    currentBulletIndex++;

    int i = 0;
    if (currentBulletIndex >= 50)
    {
        currentBulletIndex = 0;
    }
}

void AnimateSpaceShip(ME_GameObject *gameObject)
{
    if (gameObject->animate && gameObject->nFrames > 0)
    {
        u32 index = (int)(SDL_GetTicks() / TIME_BTW_FRAMES) % gameObject->nFrames;

        if (index % 2)
        {
            gameObject->srcRect.x = gameObject->srcRect.w * index;
            gameObject->srcRect.y = 0;
        }
        else
        {
            gameObject->srcRect.y = gameObject->srcRect.h;
        }
    }
    else
    {
        gameObject->srcRect.x = 0;
    }
}

void HandleEvent(SDL_Event event)
{
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    // ME_DEBUG_LOG_COORDINATES(mousePos);

    // spaceship->angle = -180 / M_PI * atan2(mousePos.x - spaceship->position.x, mousePos.y - spaceship->position.y);

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
            Shoot();
    }

    switch (event.type)
    {
    case SDL_KEYDOWN:
        switch (event.key.keysym.scancode)
        {

        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            spaceShipVel.x = -300.0f;
            break;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            spaceShipVel.x = 300.0f;
            break;

        }
        break;

    case SDL_KEYUP:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            spaceShipVel.x = 0.0f;
            break;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            spaceShipVel.x = 0.0f;
            break;

        case SDL_SCANCODE_SPACE:
            Shoot();
            break;
        }
        break;
    }
}

void Update(float deltaTime)
{   
    MUI_BeginFrame(&ui, NULL);

    AnimateSpaceShip(spaceship);
    ME_UpdateGameObject(enemySmall);
    ME_UpdateGameObject(explosion);

    if (enableTimer)
    {
        timer += 1;
    }
    else
    {
        timer = 0;
    }

    // printf("timer : %d\n", timer);

    if(timer > 20)
    {
        explosion->enabled = false;
        explosion->srcRect.x = 0;
        enableTimer = false;
    }

    int i = 0;
    for (i = 0; i < 50; i++)
    {
        bullets[i]->position.x += velocity[i] * bulletDir[i].x * deltaTime;
        bullets[i]->position.y += velocity[i] * bulletDir[i].y * deltaTime;

        //NOTE: reset bullet if gone out of bound or view
        if (bullets[i]->position.y < 0)
        {
            bullets[i]->position = spaceship->position;
            bullets[currentBulletIndex]->enabled = false;
            velocity[i] = 0;
        }

        //NOTE: follow space ship if bullet in idle
        if (velocity[i] == 0.0f)
        {
            bullets[i]->position = spaceship->position;
        }

        if (SDL_HasIntersection(&bullets[i]->destRect, &enemySmall->destRect) && enemySmall->enabled)
        {
            velocity[i] = 0;
            enemySmall->enabled = false;
            explosion->position = enemySmall->position;
            explosion->enabled = true;
            enableTimer = true;
            enemySmall->position = Vector2Init(ME_Random(30, 700), 70);
            enemySmall->enabled = true;
            score += 5;
        }
    }

    if ((spaceship->position.x - spaceship->destRect.w / 2)  > 800)
    {
        spaceship->position.x = -spaceship->destRect.w / 2;
    }

    if ((spaceship->position.x + spaceship->destRect.w / 2)  < 0)
    {
        spaceship->position.x = 800 + spaceship->destRect.w / 2;
    }

    spaceship->position.x += spaceShipVel.x * deltaTime;
    // enemySmall->position.x = spaceship->position.x;

    sprintf(scoreText, "SCORE : %d", score);
    MUI_Text(&ui, GEN_MUI_ID(), MUI_RectInit(400, 580, 120, 30), scoreText, 23);

}

void Render(SDL_Renderer *renderer)
{
    SDL_Color rendColor = ME_GetRenderColor(renderer);
    SDL_Rect rect = {0, 0, 800, 600};
    SDL_RenderCopy(renderer, background, NULL, &rect);

    SDL_Rect bottomRect = {0, 560, 800, 40};
    SDL_Color bColor = {30, 30, 30, 0};
    ME_RenderFillRect(renderer, &bottomRect, bColor);
     
    ME_RenderGameObject(spaceship, renderer);
    ME_RenderGameObject(enemySmall, renderer);
    ME_RenderGameObject(explosion, renderer);
    ME_SetRenderColor(renderer, ME_HexToSdlColor(0xff00ff));
    // SDL_RenderDrawLine(renderer, spaceship->position.x, spaceship->position.y, mousePos.x, mousePos.y);
    ME_SetRenderColor(renderer, rendColor);

    int i = 0;

    for (i = 0; i < 50; i++)
    {
        ME_RenderGameObject(bullets[i], renderer);
    }

    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{
    game = ME_CreateGame("SpaceShip game", 800, 600);
    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    SDL_Renderer *renderer = game.platform.renderer;

    //Initialization

    spaceship = ME_CreateGameObject(400, 500);
    spaceship->texture = IMG_LoadTexture(renderer, "assets/Sprites/ship.png");
    spaceship->destRect.w = 32 * 1.3;
    spaceship->destRect.h = 48 * 1.3;
    spaceship->srcRect.w = 16;
    spaceship->srcRect.h = 24;
    spaceship->animate = true;
    spaceship->nFrames = 5;

    enemySmall = ME_CreateGameObject(400, 70);
    enemySmall->texture = IMG_LoadTexture(renderer, "assets/Sprites/enemy-big.png");
    enemySmall->srcRect.w = 32;
    enemySmall->srcRect.h = 32;
    enemySmall->destRect.w = 64;
    enemySmall->destRect.h = 64;
    enemySmall->animate = true;
    enemySmall->nFrames = 2;

    explosion = ME_CreateGameObject(400, 300);
    explosion->texture =  IMG_LoadTexture(renderer, "assets/Sprites/explosion.png");
    explosion->srcRect.w = 16;
    explosion->srcRect.h = 16;
    explosion->destRect.w = 64;
    explosion->destRect.h = 64;
    explosion->animate = true;
    explosion->nFrames = 5;
    explosion->enabled = false;

    background = IMG_LoadTexture(renderer, "assets/Sprites/desert-backgorund.png");

    int i = 0;
    for (i = 0; i < 50; i++)
    {
        bullets[i] = ME_CreateGameObject(400, 300);
        bullets[i]->position = spaceship->position;
        bullets[i]->destRect.w = 16;
        bullets[i]->destRect.h = 32;
        bullets[i]->srcRect.w = 16;
        bullets[i]->srcRect.h = 32;
        bullets[i]->texture = IMG_LoadTexture(renderer, "assets/Sprites/laser-bolts.png");
        bullets[i]->enabled = false;
        bullets[i]->animate = true;
        bullets[i]->nFrames = 2;
        velocity[i] = 0;
        bulletDir[i] = Vector2Init(0, 0);
    }

    ME_RunGame(&game);

    //Cleaning up everything
    ME_DestroyGameObject(spaceship);
    for (i = 0; i < 50; i++)
    {
        ME_DestroyGameObject(bullets[i]);
    }

    ME_QuitGame(&game);

    return 0;
}
