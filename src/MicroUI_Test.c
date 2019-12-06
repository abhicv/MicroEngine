#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroUI/MicroUI.h"
#include "../../include/MIcroEngine/ME_Utility.h"

int main(int argc, char* argv[])
{
    if(!ME_Init("MicroUI test",800,600))
        return EXIT_FAILURE;

    SDL_Renderer *mainRenderer = ME_GetRenderer();
    ME_SetRenderColor(mainRenderer, ME_HexToSdlColor(0xffffff));

    bool quit = false;
    float deltaTime = 0.016f;

    MUI_Button button = MUI_CreateButton(300,100);
    //button.bgTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/Button_white.png");
    button.label.textColor = ME_HexToSdlColor(0xffff00);

    MUI_TextBox text = MUI_CreateTextBox(100,100,20);
    text.textColor = ME_HexToSdlColor(0xff00ff);

    MUI_CheckBox checkBox = MUI_CreateCheckBox(200,200);
    //checkBox.bgTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/grey_circle.png");
    checkBox.tickTexture = IMG_LoadTexture(mainRenderer, "assets/Sprites/checkmark.png");
    checkBox.checked = true;

    SDL_Event event;
    while(!quit)
    {
        ME_GetDeltaTime(&deltaTime);

        while(SDL_PollEvent(&event))
        {
            if(MUI_CheckBoxEvent(&checkBox, event))
            {
                SDL_Log("Check box event\n");
            }

            if(MUI_ButtonPressed(&button, event))
            {
                SDL_Log("Button press\n");
            }

            switch(event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        if(checkBox.checked)
            text.enabled = true;
        else
            text.enabled = false;

        SDL_RenderClear(mainRenderer);

        MUI_RenderButton(&button, mainRenderer);
        MUI_RenderTextBox(&text, mainRenderer);
        MUI_RenderCheckBox(&checkBox, mainRenderer);

        SDL_RenderPresent(mainRenderer);
    }

    MUI_DestroyButton(&button);
    MUI_DestroyTextBox(&text);
    MUI_DestroyCheckBox(&checkBox);

    ME_Quit();

    return EXIT_SUCCESS;
}
