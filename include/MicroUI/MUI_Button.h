#ifndef MUI_BUTTON_H
#define MUI_BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

#include "MUI_TextBox.h"

typedef struct
{
    SDL_Texture *bgTexture;
    SDL_Rect rect;
    MUI_TextBox label;
    bool enabled;
    const char *buttonID;

}MUI_Button;

MUI_Button MUI_CreateButton(int,int);
bool MUI_ButtonPressed(MUI_Button*, SDL_Event);
void MUI_RenderButton(MUI_Button*, SDL_Renderer*);
void MUI_DestroyButton(MUI_Button*);


#endif //MUI_BUTTON_H
