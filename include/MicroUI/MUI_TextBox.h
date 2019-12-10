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
    SDL_Texture *fontTexture;
    SDL_Rect rect;
    SDL_Color textColor;
    bool enabled;

} MUI_TextBox;

enum MUI_TextRenderMethod
{
    MUI_TEXT_SOLID,
    MUI_TEXT_BLENDED,
    MUI_TEXT_SHADED
};

MUI_TextBox MUI_CreateTextBox(int, int, int);
void MUI_RenderTextBox(MUI_TextBox*, SDL_Renderer*, enum MUI_TextRenderMethod);
void MUI_DestroyTextBox(MUI_TextBox*);

#endif //MUI_TEXTBOX_H
