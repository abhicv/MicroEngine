#ifndef MUI_TEXTBOX_H
#define MUI_TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char *textString;
    TTF_Font *font;
    SDL_Rect rect;
    SDL_Color textColor;
    bool enabled;

} MUI_TextBox;

MUI_TextBox MUI_CreateTextBox(int, int, int);
void MUI_RenderTextBox(MUI_TextBox*, SDL_Renderer*);
void MUI_DestroyTextBox(MUI_TextBox*);

#endif //MUI_TEXTBOX_H
