#include "../../include/MicroUI/MUI_Button.h"

const SDL_Color defaultButtonLabelColor = {0,0,0,255};

MUI_Button MUI_CreateButton(int x, int y)
{
    MUI_Button button;
    button.bgTexture = NULL;
    button.enabled = true;
    button.rect.x  = x;
    button.rect.y = y;
    button.rect.w = 150;
    button.rect.h = 40;

    button.label = MUI_CreateTextBox(x,y,25);
    button.label.textString = "button";

    button.label.textColor = defaultButtonLabelColor;

    return button;
}

bool MUI_ButtonPressed(MUI_Button *button, SDL_Event event)
{
    SDL_Point mousePos;

    if(button->enabled)
    {
        SDL_Rect tmpRect = button->rect;

        tmpRect.x = button->rect.x - button->rect.w / 2;
        tmpRect.y = button->rect.y - button->rect.h / 2;

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mousePos.x = event.button.x;
            mousePos.y = event.button.y;

            if(SDL_PointInRect(&mousePos,&tmpRect))
            {
                return true;
            }
        }

        return false;
    }

    return false;

}

void MUI_RenderButton(MUI_Button *button, SDL_Renderer *rend)
{
    if(button->enabled)
    {
        if(button->bgTexture != NULL)
        {
            SDL_QueryTexture(button->bgTexture, NULL, NULL, &button->rect.w, &button->rect.h);
            SDL_Rect tmpRect = button->rect;

            tmpRect.x = button->rect.x - button->rect.w / 2;
            tmpRect.y = button->rect.y - button->rect.h / 2;

            SDL_RenderCopy(rend,button->bgTexture,NULL,&tmpRect);
        }
        else
        {
            SDL_RenderDrawRect(rend,&button->rect);
        }

        //Aligning label with button center
        MUI_RenderTextBox(&button->label, rend);
    }

}

void MUI_DestroyButton(MUI_Button *button)
{
    SDL_DestroyTexture(button->bgTexture);
    button->bgTexture = NULL;

    MUI_DestroyTextBox(&button->label);
}
