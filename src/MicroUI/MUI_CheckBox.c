#include "../../include/MicroUI/MUI_CheckBox.h"

MUI_CheckBox MUI_CreateCheckBox(int x, int y)
{
    MUI_CheckBox checkBox;

    checkBox.bgTexture = NULL;
    checkBox.tickTexture = NULL;

    checkBox.rect.x = x;
    checkBox.rect.y = y;
    checkBox.rect.w = 32;
    checkBox.rect.h = 32;

    checkBox.enabled = true;
    checkBox.checked = false;

    return checkBox;
}

bool MUI_CheckBoxEvent(MUI_CheckBox *checkBox, SDL_Event event)
{
    SDL_Point mousePos;

        if(checkBox->enabled)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                mousePos.x = event.button.x;
                mousePos.y = event.button.y;

                if(SDL_PointInRect(&mousePos,&checkBox->rect))
                {
                    checkBox->checked = !checkBox->checked;
                    return true;
                }
            }

            return false;
        }

        return false;
}


void MUI_RenderCheckBox(MUI_CheckBox *checkBox, SDL_Renderer *renderer)
{
    SDL_Rect tickRect = checkBox->rect;
    tickRect.w -= 10;
    tickRect.h -= 10;

    if(checkBox->enabled)
    {
        if(checkBox->bgTexture != NULL)
        {
            SDL_QueryTexture(checkBox->bgTexture, NULL, NULL, &checkBox->rect.w, &checkBox->rect.h);
            SDL_QueryTexture(checkBox->tickTexture, NULL, NULL, &tickRect.w, &tickRect.h);

            SDL_RenderCopy(renderer, checkBox->bgTexture, NULL, &checkBox->rect);

            tickRect.x = checkBox->rect.x + (checkBox->rect.w - tickRect.w) / 2;
            tickRect.y = checkBox->rect.y + (checkBox->rect.h - tickRect.h) / 2;

            if(checkBox->checked && checkBox->tickTexture != NULL)
                SDL_RenderCopy(renderer, checkBox->tickTexture, NULL, &tickRect);
        }
        else
        {
            SDL_RenderDrawRect(renderer, &checkBox->rect);

            if(checkBox->checked)
                SDL_RenderFillRect(renderer, &tickRect);
        }
    }
}

void MUI_DestroyCheckBox(MUI_CheckBox *checkBox)
{
    SDL_DestroyTexture(checkBox->bgTexture);
    SDL_DestroyTexture(checkBox->tickTexture);

    checkBox->bgTexture = NULL;
    checkBox->tickTexture = NULL;
}
