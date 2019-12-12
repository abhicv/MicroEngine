#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroUI/MicroUI.h"
#include "../../include/MIcroEngine/ME_Utility.h"

MUI_Button button;
MUI_Button b2;
MUI_TextBox text;
MUI_CheckBox checkBox;

void HandleEvents(SDL_Event event)
{
    if(MUI_CheckBoxEvent(&checkBox, event))
    {
        SDL_Log("Check box event\n");
    }

    if(MUI_ButtonPressed(&button, event))
    {
        SDL_Log("%s Button press\n", button.label);
    }

    if(MUI_ButtonPressed(&b2, event))
    {
        SDL_Log("%s Button press\n", b2.label);
    }

}

void Update(float deltaTime)
{
    if(checkBox.checked)
        text.enabled = true;
    else
        text.enabled = false;

    sprintf(text.textString, "fps : %0.2f", 1/deltaTime);
}

void Render(SDL_Renderer *renderer)
{
    MUI_RenderButton(&button, renderer);
    MUI_RenderButton(&b2, renderer);
    MUI_RenderTextBox(&text, renderer, MUI_TEXT_SOLID);
    MUI_RenderCheckBox(&checkBox, renderer);
}

int main(int argc, char* argv[])
{
    if(!ME_Init("MicroUI test",800,600))
    {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *mainRenderer = ME_GetRenderer();;

    //UI Elements
    button = MUI_CreateButton(300,100);
    MUI_SetButtonLabelFont(&button, TTF_OpenFont(BIT_5x3_FONT_FILE,25));
    MUI_SetButtonLabel(&button, "play");

    b2 = MUI_CreateButton(300, 150);
    MUI_SetButtonLabel(&b2, "Quit");

    text = MUI_CreateTextBox(100,100,20);
    text.textColor = ME_HexToSdlColor(0xffffff);
    //text.font = TTF_OpenFont("assets/Font/OpenSans-Regular.ttf", 20);

    checkBox = MUI_CreateCheckBox(200,200);
    //checkBox.bgTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/grey_circle.png");
    //checkBox.tickTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/checkmark.png");
    checkBox.checked = true;

    ME_Run(&HandleEvents, &Update, &Render);

    MUI_DestroyButton(&button);
    MUI_DestroyButton(&b2);
    MUI_DestroyTextBox(&text);
    MUI_DestroyCheckBox(&checkBox);

    ME_Quit();

    return EXIT_SUCCESS;
}
