#include "../../include/MicroUI/MUI_TextBox.h"

#define MAX_STRING_SIZE 50
#define DEFAULT_FONT_FILE "assets/Font/bit5x3.ttf"

const SDL_Color dfTextColor = {0, 0, 0, 255};

MUI_TextBox MUI_CreateTextBox(int x, int y, int fontSize)
{
    MUI_TextBox textBox;

    textBox.rect.x = x;
    textBox.rect.y = y;

    textBox.textString = (char*)malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(textBox.textString, "TextBox");

    textBox.font = NULL;
    textBox.font = TTF_OpenFont(DEFAULT_FONT_FILE, fontSize);

    textBox.fontTexture = NULL;

    if(textBox.font == NULL)
    {
        SDL_Log("Could not load default font : %s", TTF_GetError());
    }

    textBox.textColor = dfTextColor;
    textBox.enabled = true;

    return textBox;
}

void MUI_RenderTextBox(MUI_TextBox* textBox, SDL_Renderer* rend)
{
    SDL_Surface *fontSurface = NULL;

    if(textBox->enabled)
    {
        if(textBox->font != NULL)
            fontSurface = TTF_RenderText_Solid(textBox->font, textBox->textString, textBox->textColor);

        if(fontSurface != NULL)
            textBox->fontTexture = SDL_CreateTextureFromSurface(rend, fontSurface);

        if(textBox->fontTexture != NULL)
        {
            SDL_QueryTexture(textBox->fontTexture, NULL, NULL, &textBox->rect.w, &textBox->rect.h);

            SDL_Rect tmpRect = textBox->rect;

            tmpRect.x = textBox->rect.x - textBox->rect.w / 2;
            tmpRect.y = textBox->rect.y - textBox->rect.h / 2;

            SDL_RenderCopy(rend, textBox->fontTexture, NULL, &tmpRect);
        }

        SDL_FreeSurface(fontSurface);
        SDL_Surface *fontSurface = NULL;
    }

}

void MUI_DestroyTextBox(MUI_TextBox* textBox)
{
    free(textBox->textString);
    TTF_CloseFont(textBox->font);
    SDL_DestroyTexture(textBox->fontTexture);

    textBox->textString = NULL;
    textBox->font = NULL;
    textBox->fontTexture = NULL;
}
