#include "../src/MicroEngine.c"

#define MUI_ORIGIN_ID 2021

global MUI ui;
global MUI_Input uiInput;
global SDL_Rect rect = {400, 300, 100, 100};
global SDL_Color color = {100, 100, 100, 100};

global char valueX[10] = "ValueX";
global char valueY[10] = "ValueY";
global f32 vx = 0.4f;
global f32 vy = 0.3f;

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);
}

MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {250, 200, 15},
        .highlightColor = {200, 140, 10},
        .textColor = {0, 0, 0},
        .fontSize = 14,
    },
};

MUI_Style sliderStyle = {
    .sliderStyle = {
        .bgColor = {255, 255, 255},
        .sliderColor = {255, 0, 0},
    }
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    MUI_Rect bRect = {100, 20, 200, 40};
    
    MUI_PushColumnLayout(&ui, bRect, 5);
    if (MUI_ButtonA(&ui, GEN_MUI_ID(), "change color", buttonStyle))
    {
        color = ME_RandomSdlColor();
    }
    
    MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 5);
    {
        sprintf(valueX, "X : %0.2f", vx * 1000.0f);
        MUI_TextA(&ui, GEN_MUI_ID(), valueX, 15, textStyle);
        vx = MUI_SliderA(&ui, GEN_MUI_ID(), vx, sliderStyle);
    }
    MUI_PopLayout(&ui);
    
    MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 5);
    {
        sprintf(valueY, "Y : %0.2f", vy * 1000.0f);
        MUI_TextA(&ui, GEN_MUI_ID(), valueY, 15, textStyle);
        vy = MUI_SliderA(&ui, GEN_MUI_ID(), vy, sliderStyle);
    }
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
    ME_Game game = ME_CreateGame("MicroUI test", 1600, 900);
    
    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;
    
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    ME_RunGame(&game, false);
    
    ME_QuitGame(&game);
    
    return 0;
}

