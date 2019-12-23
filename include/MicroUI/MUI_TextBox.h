#ifndef MUI_TEXTBOX_H
#define MUI_TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_SIZE 50
#define BIT_5x3_FONT_FILE "assets/Font/bit5x3.ttf"
#define OPENSANS_FONT_FILE "assets/Font/OpenSans-Regular.ttf"

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

enum MUI_TextBoxType
{
    MUI_DYNAMIC_TEXT,
    MUI_STATIC_TEXT,
};

MUI_TextBox MUI_CreateTextBox(int x, int y, int fontSize);

void MUI_SetTextBoxPosition(MUI_TextBox *textBox, int x, int y);

void MUI_SetTextBoxColor(MUI_TextBox *textBox, SDL_Color color);

void MUI_SetTextBoxFont(MUI_TextBox *textBox, TTF_Font *font);

void MUI_RenderTextBox(MUI_TextBox* textBox, SDL_Renderer* renderer, enum MUI_TextRenderMethod rendMethod);

void MUI_DestroyTextBox(MUI_TextBox* textBox);


#endif //MUI_TEXTBOX_H
