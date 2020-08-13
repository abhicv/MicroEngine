#include "../src/MicroEngine.c"

#define OBSTACLE_MAX_COUNT 4
#define DISTANCE_BTW_OBSTACLES 250

global ME_Game game = {0};
global ME_GameObject *player = NULL;
global ME_GameObject *obstacles[OBSTACLE_MAX_COUNT] = {NULL};
global SDL_Texture *backgroundLayer = NULL;

global f32 time = 0.0f;

//UI elements
global MUI_TextBox scoreText;
global MUI_TextBox gameOverText;

global Vector2 velocity = {0.0f, 0.0f};
global f32 gravity = 200.0f;
global u32 score = 0;
global u32 toCheck = 0;

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
    
    if (player->position.y > game.windowHeight)
    {
        velocity.y = 0;
        player->position.y = game.windowHeight;
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
    
    int i = 0;
    for(i = 0; i < OBSTACLE_MAX_COUNT; i++)
    {
        obstacles[i]->position.x += -200.0f * deltaTime;
        
        //NOTE(abhicv): reseting position of obstacles
        if (obstacles[i]->position.x <= -100)
        {
            obstacles[i]->position.y = ME_Random(game.windowHeight - 200, game.windowHeight + 30);
            
            if (i != 0)
            {
                obstacles[i]->position.x = obstacles[i - 1]->position.x + DISTANCE_BTW_OBSTACLES;
            }
            
            if (i == 0)
            {
                obstacles[0]->position.x = obstacles[OBSTACLE_MAX_COUNT - 1]->position.x + DISTANCE_BTW_OBSTACLES;
            }
            
            obstacles[i]->enabled = false;
        }
        else if (obstacles[i]->position.x > game.windowWidth + 100)
        {
            obstacles[i]->enabled = false;
        }
        else
        {
            obstacles[i]->enabled = true;
        }
    }
    
    //NOTE(abhicv): Checking for obstacle avoidance by player and incrementing score
    if (obstacles[toCheck]->position.x + 60 < player->position.x)
    {
        score += 1;
        toCheck += 1;
        
        if (toCheck >= OBSTACLE_MAX_COUNT)
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
    int i = 0;
    for(i = 0; i < OBSTACLE_MAX_COUNT; i++)
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
    game = ME_CreateGame("Flappy Bird clone", 400, 600);
    game.handleEvent = handleEvent;
    game.update = update;
    game.render = render;
    
    SDL_Renderer *mainRenderer = game.platform.renderer;
    
    ME_SetRenderColor(mainRenderer, ME_HexToSdlColor(0x5fcde4));
    
    player = ME_CreateGameObject(game.windowWidth / 2, game.windowHeight / 2);
    player->texture = IMG_LoadTexture(mainRenderer, "assets/Sprites/flappy.png");
    player->animate = true;
    player->nFrames = 3;
    player->srcRect.w = 34;
    player->srcRect.h = 24;
    
    player->destRect.w = 34 * 1.5;
    player->destRect.h = 24 * 1.5;
    backgroundLayer = IMG_LoadTexture(mainRenderer, "assets/Sprites/background-night.png");
    
    loop(i, OBSTACLE_MAX_COUNT)
    {
        obstacles[i] = ME_CreateGameObject(game.windowWidth + 100,
                                           ME_Random(game.windowHeight - 200, game.windowHeight));
        obstacles[i]->texture = IMG_LoadTexture(mainRenderer, "assets/Sprites/pipe-green.png");
        obstacles[i]->destRect.w = 100;
        obstacles[i]->destRect.h = 400;
        
        SDL_QueryTexture(obstacles[i]->texture, NULL, NULL, &obstacles[i]->srcRect.w, &obstacles[i]->srcRect.h);
        
        if (i != 0)
        {
            obstacles[i]->position.x = obstacles[i - 1]->position.x + DISTANCE_BTW_OBSTACLES;
        }
    }
    
    //User interface
    //Score text
    scoreText = MUI_CreateTextBox(game.windowWidth / 2, 50, 50);
    MUI_SetTextBoxColor(&scoreText, ME_HexToSdlColor(0xffffff));
    
    gameOverText = MUI_CreateTextBox(game.windowWidth / 2, 300, 60);
    MUI_SetTextBoxColor(&gameOverText, ME_HexToSdlColor(0xff0000));
    sprintf(gameOverText.textString, "GameOver");
    gameOverText.enabled = false;
    
    //game loop
    ME_RunGame(&game);
    
    //Destroying
    ME_DestroyGameObject(player);
    
    loop (n, OBSTACLE_MAX_COUNT)
    {
        ME_DestroyGameObject(obstacles[n]);
    }
    
    MUI_DestroyTextBox(&scoreText);
    ME_QuitGame(&game);
    
    return 0;
}
