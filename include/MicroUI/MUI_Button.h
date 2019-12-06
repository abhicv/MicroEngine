#ifndef MUI_BUTTON_H
#define MUI_BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdbool.h>

#include "MUI_TextBox.h"

typedef enum{

    CLICKED,
    ACTIVE,
    IDLE,

}MUI_ButtonState;

typedef struct
{
    SDL_Texture *bgTexture;
    SDL_Rect rect;
    MUI_TextBox label;
    bool enabled;
    MUI_ButtonState state;
    SDL_Color highlightColor;
    SDL_Color normalColor;

}MUI_Button;

MUI_Button MUI_CreateButton(int,int);
bool MUI_ButtonPressed(MUI_Button*, SDL_Event);
void MUI_RenderButton(MUI_Button*, SDL_Renderer*);
void MUI_DestroyButton(MUI_Button*);


#endif //MUI_BUTTON_H
