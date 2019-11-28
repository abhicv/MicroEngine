#include "../../include/MicroEngine/MicroEngine.h"
#include "../../include/MicroUI/MicroUI.h"

int main(int argc, char* argv[])
{
    if(!ME_Init("MicroUI test",800,600))
        return EXIT_FAILURE;

    bool quit = false;
    float deltaTime = 0.016f;

    MUI_Button button = MUI_CreateButton(0,0);
    button.bgTexture  = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Button_white.png");

    MUI_TextBox text = MUI_CreateTextBox(100,100,20);

    MUI_CheckBox checkBox = MUI_CreateCheckBox(200,200);
    checkBox.bgTexture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/grey_circle.png");
    checkBox.tickTexture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/checkmark.png");
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

        sprintf(text.textString, "This a text box.");

        SDL_RenderClear(ME_GetRenderer());

        MUI_RenderButton(&button, ME_GetRenderer());
        MUI_RenderTextBox(&text, ME_GetRenderer());
        MUI_RenderCheckBox(&checkBox, ME_GetRenderer());

        SDL_RenderPresent(ME_GetRenderer());
    }

    MUI_DestroyButton(&button);
    MUI_DestroyTextBox(&text);
    MUI_DestroyCheckBox(&checkBox);

    ME_Quit();

    return EXIT_SUCCESS;
}
