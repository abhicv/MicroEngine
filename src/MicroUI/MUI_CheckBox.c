#include "../../include/MicroUI/MUI_CheckBox.h"
#include "../../include/MicroEngine/ME_Utility.h"

MUI_CheckBox MUI_CreateCheckBox(int x, int y)
{
    MUI_CheckBox checkBox;

    checkBox.bgTexture = NULL;
    checkBox.tickTexture = NULL;

    checkBox.rect.x = x;
    checkBox.rect.y = y;
    checkBox.rect.w = 21;
    checkBox.rect.h = 21;

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
    tickRect.w  = checkBox->rect.w * 0.6;
    tickRect.h  = checkBox->rect.h * 0.6;

    if(checkBox->enabled)
    {
        if(checkBox->bgTexture != NULL)
        {
            tickRect.x = checkBox->rect.x + (checkBox->rect.w - tickRect.w) / 2;
            tickRect.y = checkBox->rect.y + (checkBox->rect.h - tickRect.h) / 2;

            SDL_RenderCopy(renderer, checkBox->bgTexture, NULL, &checkBox->rect);

            if(checkBox->checked && checkBox->tickTexture != NULL)
                SDL_RenderCopy(renderer, checkBox->tickTexture, NULL, &tickRect);
        }
        else
        {
            tickRect.x = checkBox->rect.x + (checkBox->rect.w - tickRect.w) / 2;
            tickRect.y = checkBox->rect.y + (checkBox->rect.h - tickRect.h) / 2;

            ME_RenderDrawRect(renderer, &checkBox->rect, ME_HexToSdlColor(0xff0000));

            if(checkBox->checked)
               ME_RenderFillRect(renderer, &tickRect, ME_HexToSdlColor(0xff0000));
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
