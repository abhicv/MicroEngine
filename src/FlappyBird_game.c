#include "../include/MicroEngine/MicroEngine.h"
#include "../include/MicroUI/MicroUI.h"
#include "../include/MicroEngine/ME_Utility.h"

//macro definition for for loop frequently used
#define foreach(count) int i = 0; \
                        for(i = 0; i < count; i++)

#define obstacle_max_count 3
#define distance_btw_obstacles 200

ME_GameObject *player;
ME_GameObject *obstacles[obstacle_max_count];

//UI elements
MUI_TextBox scoreText;
MUI_TextBox gameOverText;

Vector2 velocity = {0.0f, 0.0f};
float gravity = 200.0f;
int score = 0;
int toCheck = 0;

void HandleEvents(SDL_Event event) {

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = -300.0f;
            //gravity = -1200.0f;

        else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            UnPause();
    }

    if (event.type == SDL_KEYUP) {
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
            velocity.y = 200.0f;
        //gravity = 1000.0f;
    }
}

void Update(float deltaTime) {
    if (player->position.y > ME_GetScreenHeight()) {
        velocity.y = 0;
        player->position.y = ME_GetScreenHeight();
        gameOverText.enabled = true;
        Pause();
    }

    if (player->position.y < 0) {
        velocity.y = 0;
        player->position.y = 0;
        gameOverText.enabled = true;
        Pause();
    }

    velocity.y += gravity * deltaTime;
    player->position.y += velocity.y * deltaTime;


    foreach(obstacle_max_count) {
        obstacles[i]->position.x += -200 * deltaTime;

        //reseting position of obstacles
        if (obstacles[i]->position.x <= 0 - 100) {
            obstacles[i]->position.y = ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight() + 30);

            if (i != 0)
                obstacles[i]->position.x = obstacles[i - 1]->position.x + distance_btw_obstacles;

            if (i == 0)
                obstacles[0]->position.x = obstacles[obstacle_max_count - 1]->position.x + distance_btw_obstacles;


            obstacles[i]->enabled = false;
        } else if (obstacles[i]->position.x > ME_GetScreenWidth() + 100) {
            obstacles[i]->enabled = false;
        } else {
            obstacles[i]->enabled = true;
        }

    }

    //Checking for obstacle avoidance by player and incrementing score
    if (obstacles[toCheck]->position.x + 60 < player->position.x) {
        score++;
        toCheck++;

        if (toCheck >= obstacle_max_count)
            toCheck = 0;
    }

    sprintf(scoreText.textString, "%d", score);

}

void Render(SDL_Renderer *renderer) {
    ME_RenderGameObject(player, renderer);

    int y;

    foreach(obstacle_max_count) {
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

int main(int argc, char *argv[]) {
    //---MicroEngine initialization---
    if (!ME_Init("Flappy Bird Clone", 400, 600)) {
        ME_Quit();
        return 1;
    }

    ME_SetRenderColor(ME_GetRenderer(), ME_HexToSdlColor(0x5fcde4));

    //---Game objects Initialization---
    //player
    player = ME_CreateGameObject(ME_GetScreenWidth() / 2, ME_GetScreenHeight() / 2);
    //player->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Player.png");

    //obstacles
    foreach(obstacle_max_count) {
        obstacles[i] = ME_CreateGameObject(ME_GetScreenWidth() + 100,
                                           ME_Random(ME_GetScreenHeight() - 200, ME_GetScreenHeight()));
        obstacles[i]->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/obstacle.png");
        obstacles[i]->destRect.w = 100;
        obstacles[i]->destRect.h = 400;

        if (i != 0)
            obstacles[i]->position.x = obstacles[i - 1]->position.x + distance_btw_obstacles;
    }

    //---User interface---
    //Score text
    scoreText = MUI_CreateTextBox(ME_GetScreenWidth() / 2, 50, 50);
    MUI_SetTextBoxColor(&scoreText, ME_HexToSdlColor(0xffffff));

    gameOverText = MUI_CreateTextBox(ME_GetScreenWidth() / 2, 300, 60);
    MUI_SetTextBoxColor(&gameOverText, ME_HexToSdlColor(0xff0000));
    sprintf(gameOverText.textString, "GameOver");
    gameOverText.enabled = false;

    //Game loop
    ME_Run(&HandleEvents, &Update, &Render);

    //---Cleaning up everything---
    ME_DestroyGameObject(player);

    for (i = 0; i < obstacle_max_count; i++) {
        ME_DestroyGameObject(obstacles[i]);
    }

    MUI_DestroyTextBox(&scoreText);

    ME_Quit();

    return 0;
}
