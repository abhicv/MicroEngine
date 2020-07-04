#include "../src/MicroEngine.c"

#define MUI_ORIGIN_ID 2000

//ME_Game states
typedef enum
{
    MENU,
    PLAY_MODE

} GameState;

global ME_Game game = {0};
global MUI ui = {.fontFile = BIT_5x3_FONT_FILE};
global MUI_Input uiInput = {0};
global GameState gameState = MENU;
global bool pause = false;

global Vector2 ballVelocity = {400.0f, 300.0f};
global f32 playerSpeed = 0;

global u32 pScore = 0; //player score
global u32 cScore = 0; //computer score

SDL_Color white = {255, 255, 255, 255};

//UI elements
global char playerScoreText[10] = "0";
global char computerScoreText[10] = "0";

global f32 computerPerformance = 5.0f;

//ME_Game objects
SDL_Texture *mainTexture = NULL;
ME_GameObject *player = NULL;
ME_GameObject *computer = NULL;
ME_GameObject *ball = NULL;

//Background grid
SDL_Texture *grid = NULL;
SDL_Rect gridRect = {0};

Vector2 CalculateMovDir(Vector2 velocity, Vector2 position)
{
    Vector2 tmpVelocity = velocity;
    Vector2 movDir = velocity;

    if (position.x <= 0.0)
        movDir = ReflectDir(tmpVelocity, NewVector2(1, 0));

    else if (position.x + 15.0 > game.windowWidth)
        movDir = ReflectDir(tmpVelocity, NewVector2(-1, 0));

    else if (position.y <= 20.0)
        movDir = ReflectDir(tmpVelocity, NewVector2(0, 1));

    else if (position.y + 15.0 > game.windowHeight - 15)
        movDir = ReflectDir(tmpVelocity, NewVector2(0, -1));

    Vector2Normalize(&movDir);
    Vector2Scale(&movDir, 400);

    return movDir;
}

void ResetBall(Vector2 *position, Vector2 *velocity)
{
    position->x = 400;
    position->y = 300;

    int angle = 0;
    angle = rand() % 120 + 30;

    velocity->x = cos(angle);
    velocity->y = sin(angle);

    Vector2Scale(velocity, 400);
}

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);

    switch (event.type)
    {
    case SDL_KEYDOWN:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            playerSpeed = -600;
            break;

        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            break;

        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            playerSpeed = 600;
            break;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            break;

        case SDL_SCANCODE_SPACE:
            pause = !pause;
            break;
        }
        break;

    case SDL_KEYUP:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP:
            playerSpeed = 0;
            break;

        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            break;

        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            playerSpeed = 0;
            break;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            break;
        }
        break;
    }
}

void Update(float deltaTime)
{

    MUI_BeginFrame(&ui, &uiInput);

    switch (gameState)
    {
    case PLAY_MODE:

        if (pause)
            deltaTime = 0.0f;

        if (ball->position.x <= 0)
        {
            pScore++;
            sprintf(playerScoreText, "%d", pScore);
            ResetBall(&ball->position, &ballVelocity);
            // computerPerformance = ME_Random(2, 6) * 1.0f;
            SDL_Delay(500);
        }
        else if (ball->position.x >= game.windowWidth - 15)
        {
            cScore++;
            sprintf(computerScoreText, "%d", cScore);
            ResetBall(&ball->position, &ballVelocity);
            // computerPerformance = ME_Random(2, 6) * 1.0f;
            SDL_Delay(500);
        }
        else if (SDL_HasIntersection(&player->destRect, &ball->destRect))
        {
            ballVelocity = ReflectDir(ballVelocity, NewVector2(-1, 0));
        }
        else if (SDL_HasIntersection(&computer->destRect, &ball->destRect))
        {
            ballVelocity = ReflectDir(ballVelocity, NewVector2(1, 0));
        }
        else
            ballVelocity = CalculateMovDir(ballVelocity, ball->position);

        ball->position.x += ballVelocity.x * deltaTime;
        ball->position.y += ballVelocity.y * deltaTime;
        player->position.y += playerSpeed * deltaTime;

        computer->position.y += (ball->position.y - computer->position.y) * computerPerformance * deltaTime;

        ME_UpdateGameObject(player);
        ME_UpdateGameObject(computer);
        ME_UpdateGameObject(ball);

        MUI_TextP(&ui, GEN_MUI_ID(),
                  MUI_RectInit(game.windowWidth / 2 + 50, 60, 120, 30),
                  playerScoreText, 60);

        MUI_TextP(&ui, GEN_MUI_ID(),
                  MUI_RectInit(game.windowWidth / 2 - 50, 60, 120, 30),
                  computerScoreText, 60);

        break;
    case MENU:

        MUI_TextP(&ui, GEN_MUI_ID(),
                  MUI_RectInit(game.windowWidth / 2, game.windowHeight / 2 - 100, 120, 30), "Pong", 140);

        MUI_TextP(&ui, GEN_MUI_ID(),
                  MUI_RectInit(game.windowWidth / 2, game.windowHeight / 2 + 100, 120, 30),
                  "Made with MicroEngine", 20);

        if (MUI_ButtonP(&ui, GEN_MUI_ID(), "Play",
                        MUI_RectInit(game.windowWidth / 2, game.windowHeight / 2, 120, 30)))
        {
            gameState = PLAY_MODE;
        }

        break;
    default:
        break;
    }
}

void Render(SDL_Renderer *renderer)
{
    switch (gameState)
    {
    case MENU:

        break;

    case PLAY_MODE:
        //Rendering GameObjects
        ME_RenderGameObject(player, renderer);
        ME_RenderGameObject(computer, renderer);
        ME_RenderGameObject(ball, renderer);

        SDL_RenderCopy(renderer, grid, NULL, &gridRect);
        break;
    }

    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{

    game = ME_CreateGame("Pong", 800, 600);
    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    SDL_Renderer *mainRenderer = game.platform.renderer;

    gridRect.w = game.windowWidth;
    gridRect.h = game.windowHeight;
    gridRect.x = gridRect.y = 0;

    //ME_Game objects initialization
    mainTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/pongTexture.png");

    player = ME_CreateGameObject(game.windowWidth - 20, game.windowHeight / 2);
    player->destRect.w = 20;
    player->destRect.h = 120;
    player->texture = mainTexture;

    computer = ME_CreateGameObject(20, game.windowHeight / 2 - 60);
    computer->destRect.w = 20;
    computer->destRect.h = 120;
    computer->texture = mainTexture;

    ball = ME_CreateGameObject(game.windowWidth / 2, game.windowHeight / 2);
    ball->destRect.w = 15;
    ball->destRect.h = 15;
    ball->texture = mainTexture;

    //Background grid
    grid = IMG_LoadTexture(mainRenderer, "assets/Sprites/grid.png");

    ME_RunGame(&game);

    //Cleaning up everything
    ME_DestroyGameObject(player);
    ME_DestroyGameObject(computer);
    ME_DestroyGameObject(ball);

    ME_QuitGame(&game);

    return 0;
}
