#include "../../include/MicroUI/MUI_TextBox.h"

#define MAX_STRING_SIZE 50
#define DEFAULT_FONT_FILE "assets/Font/OpenSans-Regular.ttf"

MUI_TextBox MUI_CreateTextBox(int x, int y, int fontSize)
{
    MUI_TextBox textBox;

    textBox.rect.x = x;
    textBox.rect.y = y;

    textBox.textString = (char*)malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(textBox.textString, "text box");

    textBox.font = NULL;
    textBox.font = TTF_OpenFont(DEFAULT_FONT_FILE, fontSize);

    if(textBox.font == NULL)
        SDL_Log("Could not load default font : %s", TTF_GetError());

    SDL_Color textColor = {0,0,0,255};
    textBox.textColor = textColor;
    textBox.enabled = true;

    return textBox;
}

void MUI_RenderTextBox(MUI_TextBox* textBox, SDL_Renderer* rend)
{
    SDL_Surface *fontSurface = NULL;
    SDL_Texture *fontTexture = NULL;

    if(textBox->enabled)
    {
        if(textBox->font != NULL)
            fontSurface = TTF_RenderText_Blended(textBox->font, textBox->textString, textBox->textColor);

        if(fontSurface != NULL)
            fontTexture = SDL_CreateTextureFromSurface(rend, fontSurface);

        if(fontTexture != NULL)
        {
            SDL_QueryTexture(fontTexture, NULL, NULL, &textBox->rect.w, &textBox->rect.h);

            SDL_Rect tmpRect = textBox->rect;

            tmpRect.x = textBox->rect.x - textBox->rect.w / 2;
            tmpRect.y = textBox->rect.y - textBox->rect.h / 2;

            //SDL_RenderDrawRect(rend, &tmpRect);
            SDL_RenderCopy(rend, fontTexture, NULL, &tmpRect);
        }

        SDL_FreeSurface(fontSurface);
        SDL_DestroyTexture(fontTexture);
    }

}

void MUI_DestroyTextBox(MUI_TextBox* textBox)
{
    free(textBox->textString);
    TTF_CloseFont(textBox->font);

    textBox->textString = NULL;
    textBox->font = NULL;
}
