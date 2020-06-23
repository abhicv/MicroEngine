#include "../src/MicroEngine.c"

//Game states
typedef enum
{
    MENU,
    PLAY_MODE
} GameState;

global GameState gameState = MENU;
global bool pause = false;

global Vector2 ballVelocity = {400.0f, 400.0f};
global float playerSpeed = 0;

int pScore = 0;
int cScore = 0;

SDL_Color white = {255, 255, 255, 255};

//UI elements
MUI_Button play;
MUI_TextBox pongTitle;
MUI_TextBox creditText;
MUI_TextBox playerScore;
MUI_TextBox computerScore;

//Game objects
SDL_Texture *mainTexture;
ME_GameObject *player;
ME_GameObject *computer;
ME_GameObject *ball;

//Background grid
SDL_Texture *grid;
SDL_Rect gridRect;

Vector2 CalculateMovDir(Vector2 velocity, Vector2 position)
{
    Vector2 tmpVelocity = velocity;
    Vector2 movDir = velocity;

    if (position.x <= 0.0)
        movDir = ReflectDir(tmpVelocity, NewVector2(1, 0));

    else if (position.x + 15.0 > ME_GetScreenWidth())
        movDir = ReflectDir(tmpVelocity, NewVector2(-1, 0));

    else if (position.y <= 20.0)
        movDir = ReflectDir(tmpVelocity, NewVector2(0, 1));

    else if (position.y + 15.0 > ME_GetScreenHeight() - 15)
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
    if (MUI_ButtonPressed(&play, event))
        gameState = PLAY_MODE;

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
    switch (gameState)
    {
    case PLAY_MODE:

        if (pause)
            deltaTime = 0.0;

        if (ball->position.x <= 0)
        {
            pScore++;
            sprintf(playerScore.textString, "%d", pScore);
            ResetBall(&ball->position, &ballVelocity);

            SDL_Delay(500);
        }
        else if (ball->position.x >= ME_GetScreenWidth() - 15)
        {
            cScore++;
            sprintf(computerScore.textString, "%d", cScore);
            ResetBall(&ball->position, &ballVelocity);

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
        computer->position.y += (ball->position.y - computer->position.y) * 5 * deltaTime;

        ME_UpdateGameObject(player);
        ME_UpdateGameObject(computer);
        ME_UpdateGameObject(ball);
    }
}

void Render(SDL_Renderer *renderer)
{
    switch (gameState)
    {
    case MENU:

        MUI_RenderButton(&play, renderer);
        MUI_RenderTextBox(&creditText, renderer, MUI_TEXT_SOLID);
        MUI_RenderTextBox(&pongTitle, renderer, MUI_TEXT_SOLID);

        break;

    case PLAY_MODE:
        //Rendering GameObjects
        ME_RenderGameObject(player, renderer);
        ME_RenderGameObject(computer, renderer);
        ME_RenderGameObject(ball, renderer);

        SDL_RenderCopy(renderer, grid, NULL, &gridRect);

        //Rendering UI
        MUI_RenderTextBox(&playerScore, renderer, MUI_TEXT_SOLID);
        MUI_RenderTextBox(&computerScore, renderer, MUI_TEXT_SOLID);

        break;
    }
}

int main(int argc, char *argv[])
{
    //Initialization of micro engine
    if (!ME_Init("Pong Game", 800, 600))
    {
        ME_Quit();
        return 1;
    }

    SDL_Renderer *mainRenderer = ME_GetRenderer();

    gridRect.w = ME_GetScreenWidth();
    gridRect.h = ME_GetScreenHeight();
    gridRect.x = gridRect.y = 0;

    //UI elements initialization
    play = MUI_CreateButton(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    MUI_SetButtonLabel(&play, "Play");

    pongTitle = MUI_CreateTextBox(play.rect.x, play.rect.y - 100, 140);
    strcpy(pongTitle.textString, "PONG");
    pongTitle.textColor = white;

    creditText = MUI_CreateTextBox(play.rect.x, play.rect.y + 200, 20);
    strcpy(creditText.textString, "Made With MicroEngine");
    creditText.textColor = white;

    playerScore = MUI_CreateTextBox(ME_GetScreenWidth() / 2 + 50, 60, 60);
    sprintf(playerScore.textString, "0");
    playerScore.textColor = white;

    computerScore = MUI_CreateTextBox(ME_GetScreenWidth() / 2 - 50, 60, 60);
    sprintf(computerScore.textString, "0");
    computerScore.textColor = white;

    //Game objects initialization
    mainTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/pongTexture.png");

    player = ME_CreateGameObject(ME_GetScreenWidth() - 20, ME_GetScreenHeight() / 2);
    player->destRect.w = 20;
    player->destRect.h = 120;
    player->texture = mainTexture;

    computer = ME_CreateGameObject(20, ME_GetScreenHeight() / 2 - 60);
    computer->destRect.w = 20;
    computer->destRect.h = 120;
    computer->texture = mainTexture;

    ball = ME_CreateGameObject(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    ball->destRect.w = 15;
    ball->destRect.h = 15;
    ball->texture = mainTexture;

    //Background grid
    grid = IMG_LoadTexture(mainRenderer, "assets/Sprites/grid.png");

    ME_Run(HandleEvent, Update, Render);

    //Cleaning up everything
    ME_DestroyGameObject(player);
    ME_DestroyGameObject(computer);
    ME_DestroyGameObject(ball);

    //Cleaning up gui components
    MUI_DestroyButton(&play);
    MUI_DestroyTextBox(&playerScore);
    MUI_DestroyTextBox(&computerScore);
    MUI_DestroyTextBox(&pongTitle);
    MUI_DestroyTextBox(&creditText);

    ME_Quit();

    return 0;
}
