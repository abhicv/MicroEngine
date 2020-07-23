#include "../src/MicroEngine.c"

#define MUI_ORIGIN_ID 3000

global MUI ui = {.fontFile = BIT_5x3_FONT_FILE};
global MUI_Input uiInput = {0};
global SDL_Rect rect = {400, 300, 100, 100};
global SDL_Color color = {100, 100, 100, 100};

global char valueX[10] = "ValueX";
global char valueY[10] = "ValueY";

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);
}

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);

    MUI_Rect bRect = {200, 30, 120, 30};

    local_persist f32 vx = 0.4f;
    local_persist f32 vy = 0.3f;

    MUI_PushColumnLayout(&ui, bRect, 5);
    if (MUI_ButtonA(&ui, GEN_MUI_ID(), "change color"))
    {
        color = ME_RandomSdlColor();
    }

    MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 5);
    sprintf(valueX, "X = %0.2f", vx * 1000.0f);
    MUI_TextA(&ui, GEN_MUI_ID(), valueX, 15);
    vx = MUI_SliderA(&ui, GEN_MUI_ID(), vx);
    MUI_PopLayout(&ui);

    MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 5);
    sprintf(valueY, "Y = %0.2f", vy * 1000.0f);
    MUI_TextA(&ui, GEN_MUI_ID(), valueY, 15);
    vy = MUI_SliderA(&ui, GEN_MUI_ID(), vy);
    MUI_PopLayout(&ui);

    rect.x = vx * 1000.0f;
    rect.y = vy * 1000.0f;

    MUI_PopLayout(&ui);
}

void Render(SDL_Renderer *renderer)
{
    ME_RenderFillRect(renderer, &rect, color);
    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{
    //Intialization
    ME_Game game = ME_CreateGame("MicroUI test", 1280, 720);

    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    ME_RunGame(&game);

    ME_QuitGame(&game);

    return 0;
}
