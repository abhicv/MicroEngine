#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroUI/MicroUI.h"
#include "../../include/MicroEngine/ME_Utility.h"

#define MENU 0
#define PLAYMODE 1

int GameState = MENU;

Vector2 ReflectDir(Vector2 I,Vector2 N)
{
    DATATYPE dotIN =  2* Vector2Dot(I,N);
    Vector2Scale(&N,dotIN);

    return Vector2Subtract(I,N);
}

Vector2 CalculateMovDir(Vector2 velocity, Vector2 position)
{
    Vector2 tmpVelocity = velocity;
    Vector2 movDir = velocity;

    if(position.x <= 0)
        movDir = ReflectDir(tmpVelocity,NewVector2(1,0));

    else if(position.x + 15.0 > ME_GetScreenWidth())
        movDir = ReflectDir(tmpVelocity,NewVector2(-1,0));

    else if(position.y <= 20)
        movDir = ReflectDir(tmpVelocity,NewVector2(0,1));

    else if(position.y + 15.0 > ME_GetScreenHeight() - 15)
        movDir = ReflectDir(tmpVelocity,NewVector2(0,-1));

    Vector2Normalize(&movDir);
    Vector2Scale(&movDir,500);
    return movDir;
}

int main(int argc, char *argv[])
{
    //Initialization
    if(!ME_Init("Pong Game",800,600))
        return EXIT_FAILURE;

    float deltaTime = 0.016f;
    bool quit = false;

    Vector2 ballVelocity = NewVector2(300,300);
    float playerSpeed = 0;

    int pScore = 0;
    int cScore = 0;

    SDL_Color white = {255,255,255,255};

    //UI elements
    MUI_Button play = MUI_CreateButton(ME_GetScreenWidth()/2,ME_GetScreenHeight()/2);
    //play.bgTexture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/pongTexture.png");
    play.label.textString = "PLAY";

    MUI_TextBox pongTitle = MUI_CreateTextBox(play.rect.x, play.rect.y - 100, 140);
    pongTitle.textString = "PONG";
    pongTitle.textColor = white;

    MUI_TextBox creditText = MUI_CreateTextBox(play.rect.x, play.rect.y + 200, 20);
    creditText.textString = "Made with MicroEngine";
    creditText.textColor = white;

    MUI_TextBox playerScore = MUI_CreateTextBox(ME_GetScreenWidth()/2 + 50, 60, 60);
    sprintf(playerScore.textString,"0");
    playerScore.textColor = white;

    MUI_TextBox computerScore = MUI_CreateTextBox(ME_GetScreenWidth()/2 - 50, 60, 60);
    sprintf(computerScore.textString,"0");
    computerScore.textColor = white;

    //Game objects
    SDL_Texture *mainTexture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/pongTexture.png");

    ME_GameObject *player = ME_CreateGameObject(ME_GetScreenWidth() - 20,ME_GetScreenHeight() / 2);
    player->destRect.w = 20;
    player->destRect.h = 120;
    player->texture = mainTexture;

    ME_GameObject *computer = ME_CreateGameObject(20,ME_GetScreenHeight() / 2 - 60);
    computer->destRect.w = 20;
    computer->destRect.h = 120;
    computer->texture = mainTexture;

    ME_GameObject *ball = ME_CreateGameObject(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    ball->destRect.w = 15;
    ball->destRect.h = 15;
    ball->texture = mainTexture;

    SDL_Texture *grid = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/grid.png");
    SDL_Rect gridRect = {0,0,800,600};

    SDL_Renderer *mainRenderer = ME_GetRenderer();

    //Main Game loop
    while(!quit)
    {
        ME_GetDeltaTime(&deltaTime);

        //Input Handling
        SDL_Event event;
        if(SDL_PollEvent(&event))
        {
            if(MUI_ButtonPressed(&play,event))
                GameState = PLAYMODE;

            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            }

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

                case SDL_SCANCODE_ESCAPE:
                    quit = true;
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

        switch(GameState)
        {
        case MENU:
            SDL_RenderClear(mainRenderer);

            MUI_RenderButton(&play,mainRenderer);
            MUI_RenderTextBox(&creditText,mainRenderer);
            MUI_RenderTextBox(&pongTitle,mainRenderer);

            SDL_RenderPresent(mainRenderer);
            break;

        case PLAYMODE:

            ball->position.x += ballVelocity.x * deltaTime;
            ball->position.y += ballVelocity.y * deltaTime;

            player->position.y += playerSpeed * deltaTime;

            computer->position.y += (ball->position.y - computer->position.y) * 2 * deltaTime;

            ballVelocity = CalculateMovDir(ballVelocity, ball->position);
            //printf("x : %f, y : %f\n",velocity.x, velocity.y);

            if(ball->position.x < 0)
            {
                pScore++;
                sprintf(playerScore.textString,"%d", pScore);
                ball->position.x = 400;
                ball->position.y = 300;

                ballVelocity.x = rand();
                ballVelocity.y = rand();

                Vector2Normalize(&ballVelocity);
                Vector2Scale(&ballVelocity,500);

                SDL_Delay(1000);
            }

            if(ball->position.x >= ME_GetScreenWidth() - 15)
            {
                cScore++;
                sprintf(computerScore.textString,"%d", cScore);
                ball->position.x = 400;
                ball->position.y = 300;

                ballVelocity.x = rand();
                ballVelocity.y = rand();

                Vector2Normalize(&ballVelocity);
                Vector2Scale(&ballVelocity,500);

                SDL_Delay(1000);
            }

            ME_UpdateGameObject(player);
            ME_UpdateGameObject(computer);
            ME_UpdateGameObject(ball);

            if(SDL_HasIntersection(&player->destRect,&ball->destRect))
                ballVelocity = ReflectDir(ballVelocity,NewVector2(-1,0));

            if(SDL_HasIntersection(&computer->destRect,&ball->destRect))
                ballVelocity = ReflectDir(ballVelocity,NewVector2(1,0));


            //RENDERING
            SDL_RenderClear(mainRenderer);

            //Rendering GameObjects
            ME_RenderGameObject(player, mainRenderer);
            ME_RenderGameObject(computer, mainRenderer);
            ME_RenderGameObject(ball, mainRenderer);

            MUI_RenderTextBox(&playerScore,mainRenderer);
            MUI_RenderTextBox(&computerScore,mainRenderer);

            SDL_RenderCopy(mainRenderer, grid, NULL, &gridRect);

            SDL_RenderPresent(mainRenderer);
            break;
        }
    }

    //Cleaning up everything
    ME_DestroyGameObject(player);
    ME_DestroyGameObject(computer);
    ME_DestroyGameObject(ball);

    MUI_DestroyButton(&play);
    MUI_DestroyTextBox(&playerScore);
    MUI_DestroyTextBox(&computerScore);
    MUI_DestroyTextBox(&pongTitle);
    MUI_DestroyTextBox(&creditText);

    ME_Quit();

    return EXIT_SUCCESS;
}
