#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroUI/MicroUI.h"

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

    else if(position.y <= 0)
        movDir = ReflectDir(tmpVelocity,NewVector2(0,1));

    else if(position.y + 15.0 > ME_GetScreenHeight())
        movDir = ReflectDir(tmpVelocity,NewVector2(0,-1));

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

    SDL_Color white = {255,255,255,255};

    //UI elements
    MUI_Button play = MUI_CreateButton(ME_GetScreenWidth()/2,ME_GetScreenHeight()/2);
    play.bgTexture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Button_white.png");
    play.label.textString = "PLAY";
    //play.label.font = TTF_OpenFont("assets/Font/OpenSans-ExtraBold.ttf",30);

    MUI_TextBox pongTitle = MUI_CreateTextBox(play.rect.x, play.rect.y - 100, 100);
    pongTitle.textString = "PONG";
    pongTitle.textColor = white;

    MUI_TextBox creditText = MUI_CreateTextBox(play.rect.x, play.rect.y + 60, 20);
    creditText.textString = "Made with MicroEngine";
    creditText.textColor = white;

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


    //Main Game loop
    while(!quit)
    {
        ME_GetDeltaTime(&deltaTime);
        //printf("%f\n",deltaTime);

        //Input Handling
        SDL_Event event;
        while (SDL_PollEvent(&event))
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
            SDL_RenderClear(ME_GetRenderer());

            MUI_RenderButton(&play,ME_GetRenderer());
            MUI_RenderTextBox(&creditText,ME_GetRenderer());
            MUI_RenderTextBox(&pongTitle,ME_GetRenderer());

            SDL_RenderPresent(ME_GetRenderer());
            break;

        case PLAYMODE:

            ball->position.x += ballVelocity.x * deltaTime;
            ball->position.y += ballVelocity.y * deltaTime;

            player->position.y += playerSpeed * deltaTime;

            computer->position.y = ball->position.y;

            ballVelocity = CalculateMovDir(ballVelocity, ball->position);
            //printf("x : %f, y : %f\n",velocity.x, velocity.y);

            ME_UpdateGameObject(player);
            ME_UpdateGameObject(computer);
            ME_UpdateGameObject(ball);

            if(SDL_HasIntersection(&player->destRect,&ball->destRect))
                ballVelocity = ReflectDir(ballVelocity,NewVector2(-1,0));

            if(SDL_HasIntersection(&computer->destRect,&ball->destRect))
                ballVelocity = ReflectDir(ballVelocity,NewVector2(1,0));


            //RENDERING
            SDL_RenderClear(ME_GetRenderer());

            //Rendering GameObjects
            ME_RenderGameObject(player);
            ME_RenderGameObject(computer);
            ME_RenderGameObject(ball);

            SDL_RenderPresent(ME_GetRenderer());
            break;
        }
    }

    //Cleaning up everything
    ME_DestroyGameObject(player);
    ME_DestroyGameObject(computer);
    ME_DestroyGameObject(ball);

    MUI_DestroyButton(&play);

    ME_Quit();

    return EXIT_SUCCESS;
}
