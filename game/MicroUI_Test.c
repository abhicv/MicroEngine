#include "../src/MicroEngine.c"

global Game game = {0};
global MUI ui = {0};
global MUI_Input uiInput = {0};
global SDL_Rect rect = {400, 300, 100, 100};
global SDL_Color color = {100, 100, 100, 100};

void handleEvent(SDL_Event event)
{
    uiInput.mouseX = event.motion.x;
    uiInput.mouseY = event.motion.y;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            uiInput.leftMouseButtonDown = true;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            uiInput.rightMouseButtonDown = true;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            uiInput.leftMouseButtonDown = false;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            uiInput.rightMouseButtonDown = false;
        }
    }
}

void update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);

    MUI_Rect bRect = {200, 30, 120, 30};
    MUI_Rect b2Rect = {200, 80, 120, 30};
    MUI_Id id = {10, 10};
    MUI_Id id2 = {20, 20};

    if (MUI_ButtonP(&ui, id, "Hello", bRect))
    {
        printf("button pressed : %d\n", id.primary);
        color = ME_RandomSdlColor();
    }

    local_persist f32 v = 0;
    v = MUI_SliderP(&ui, id2, v, b2Rect);
    rect.w = v * 100.0f;
    rect.h = v * 100.0f;
}

void render(SDL_Renderer *renderer)
{
    MUI_EndFrame(&ui, renderer);
    // ME_RenderDrawRect(renderer, &rect, color);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char *argv[])
{
    //Intialization
    Game game = ME_CreateGame("MicroUI test", 800, 600);

    game.handleEvent = handleEvent;
    game.update = update;
    game.render = render;

    SDL_Renderer *mainRenderer = ME_GetRenderer();

    ME_RunGame(&game);

    ME_Quit();

    return 0;
}
