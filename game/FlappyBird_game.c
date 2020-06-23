#include "../src/MicroEngine.c"

//macro definition 'for loop'
#define foreach(i, count) \
    int i = 0;            \
    for (i = 0; i < count; i++)

#define obstacle_max_count 4
#define distance_btw_obstacles 250

global ME_GameObject *player = NULL;
global ME_GameObject *obstacles[obstacle_max_count] = {NULL};
global SDL_Texture *backgroundLayer = NULL;

global f32 time = 0.0f;

//UI elements
static MUI_TextBox scoreText;
static MUI_TextBox gameOverText;

static Vector2 velocity = {0.0f, 0.0f};
static f32 gravity = 200.0f;
static u32 score = 0;
static u32 toCheck = 0;

void handleEvent(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = -300.0f;
        //gravity = -1200.0f;
    }

    if (event.type == SDL_KEYUP)
    {
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = 200.0f;
        //gravity = 1000.0f;
    }
}

void update(float deltaTime)
{
    time = time + deltaTime;
    scoreText.rect.y = 50.0f + 10.0f * sinf(time * 2.0f);

    ME_UpdateGameObject(player);

    if (player->position.y > ME_GetScreenHeight())
    {
        velocity.y = 0;
        player->position.y = ME_GetScreenHeight();
        gameOverText.enabled = true;
    }

    if (player->position.y < 0.0f)
    {
        velocity.y = 0.0f;
        player->position.y = 0.0f;
        gameOverText.enabled = true;
    }

    velocity.y += gravity * deltaTime;
    player->position.y += velocity.y * deltaTime;

    foreach (i, obstacle_max_count)
    {
        obstacles[i]->position.x += -200.0f * deltaTime;

        //reseting position of obstacles
        if (obstacles[i]->position.x <= -100)
        {
            obstacles[i]->position.y = ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight() + 30);

            if (i != 0)
            {
                obstacles[i]->position.x = obstacles[i - 1]->position.x + distance_btw_obstacles;
            }

            if (i == 0)
            {
                obstacles[0]->position.x = obstacles[obstacle_max_count - 1]->position.x + distance_btw_obstacles;
            }

            obstacles[i]->enabled = false;
        }
        else if (obstacles[i]->position.x > ME_GetScreenWidth() + 100)
        {
            obstacles[i]->enabled = false;
        }
        else
        {
            obstacles[i]->enabled = true;
        }
    }

    //Checking for obstacle avoidance by player and incrementing score
    if (obstacles[toCheck]->position.x + 60 < player->position.x)
    {
        score += 1;
        toCheck += 1;

        if (toCheck >= obstacle_max_count)
            toCheck = 0;
    }

    sprintf(scoreText.textString, "%d", score);
}

void render(SDL_Renderer *renderer)
{
    SDL_Rect bgSrc = {0, 0, 0, 0};
    SDL_Rect bgDest = {0, 0, 400, 600};

    SDL_QueryTexture(backgroundLayer, NULL, NULL, &bgSrc.w, &bgSrc.h);
    SDL_RenderCopyEx(renderer, backgroundLayer, &bgSrc, &bgDest,
                     0, NULL, SDL_FLIP_NONE);

    ME_RenderGameObject(player, renderer);

    int y = 0;

    foreach (i, obstacle_max_count)
    {
        ME_RenderGameObject(obstacles[i], renderer);
        y = obstacles[i]->position.y;

        obstacles[i]->position.y -= 550;
        obstacles[i]->angle = 180.0f;
        ME_RenderGameObject(obstacles[i], renderer);
        obstacles[i]->angle = 0;
        obstacles[i]->position.y = y;
    }

    MUI_RenderTextBox(&scoreText, renderer, MUI_TEXT_SOLID);
    MUI_RenderTextBox(&gameOverText, renderer, MUI_TEXT_SOLID);
}

int main(int argc, char *argv[])
{
    //---MicroEngine initialization---
    if (!ME_Init("Flappy Bird Clone", 400, 600))
    {
        ME_Quit();
        return 1;
    }

    ME_SetRenderColor(ME_GetRenderer(), ME_HexToSdlColor(0x5fcde4));

    //Game objects Initialization
    //player
    player = ME_CreateGameObject(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    player->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/flappy.png");
    player->animate = true;
    player->nFrames = 3;
    player->srcRect.w = 34;
    player->srcRect.h = 24;

    player->destRect.w = 34 * 1.5;
    player->destRect.h = 24 * 1.5;
    backgroundLayer = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/background-night.png");

    //obstacles
    foreach (i, obstacle_max_count)
    {
        obstacles[i] = ME_CreateGameObject(ME_GetScreenWidth() + 100,
                                           ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight()));
        obstacles[i]->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/pipe-green.png");
        obstacles[i]->destRect.w = 100;
        obstacles[i]->destRect.h = 400;

        SDL_QueryTexture(obstacles[i]->texture, NULL, NULL, &obstacles[i]->srcRect.w, &obstacles[i]->srcRect.h);

        if (i != 0)
        {
            obstacles[i]->position.x = obstacles[i - 1]->position.x + distance_btw_obstacles;
        }
    }

    //User interface
    //Score text
    scoreText = MUI_CreateTextBox(ME_GetScreenWidth() / 2, 50, 50);
    MUI_SetTextBoxColor(&scoreText, ME_HexToSdlColor(0xffffff));

    gameOverText = MUI_CreateTextBox(ME_GetScreenWidth() / 2, 300, 60);
    MUI_SetTextBoxColor(&gameOverText, ME_HexToSdlColor(0xff0000));
    sprintf(gameOverText.textString, "GameOver");
    gameOverText.enabled = false;

    //Game loop
    ME_Run(handleEvent, update, render);

    //Destroying        
    ME_DestroyGameObject(player);

    foreach (n, obstacle_max_count)
    {
        ME_DestroyGameObject(obstacles[n]);
    }

    MUI_DestroyTextBox(&scoreText);
    ME_Quit();

    return 0;
}
