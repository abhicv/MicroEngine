#include "../../include/MicroUI/MUI_Button.h"
#include "../../include/MicroEngine/ME_Utility.h"

#define DEFAULT_BUTTON_WIDTH 100
#define DEFAULT_BUTTON_HEIGHT 30

SDL_Color BUTTON_NORMAL_COLOR  = {255, 255, 255, 255};
SDL_Color BUTTON_ACTIVE_COLOR  = {0, 0, 255, 255};

MUI_Button MUI_CreateButton(int x, int y)
{
    MUI_Button button;

    button.bgTexture = NULL;
    button.enabled = true;
    button.state = IDLE;

    button.rect.x  = x;
    button.rect.y = y;
    button.rect.w = DEFAULT_BUTTON_WIDTH;
    button.rect.h = DEFAULT_BUTTON_HEIGHT;

    button.label = MUI_CreateTextBox(x,y,20);
    strcpy(button.label.textString,"Hello");

    button.normalColor = BUTTON_NORMAL_COLOR;
    button.highlightColor = BUTTON_ACTIVE_COLOR;

    return button;
}

bool MUI_ButtonPressed(MUI_Button *button, SDL_Event event)
{
    SDL_Point mousePos;

    if(button->enabled)
    {
        SDL_Rect tmpRect = button->rect;

        mousePos.x = event.motion.x;
        mousePos.y = event.motion.y;

        tmpRect.x = button->rect.x - button->rect.w / 2;
        tmpRect.y = button->rect.y - button->rect.h / 2;

        if(SDL_PointInRect(&mousePos,&tmpRect))
        {
            button->state = ACTIVE;

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                button->state = CLICKED;
                return true;
            }
        }
        else
        {
            button->state = IDLE;
        }
    }

    return false;

}

void MUI_RenderButton(MUI_Button *button, SDL_Renderer *rend)
{
    SDL_Color presentColor;

    if(button->enabled)
    {
        SDL_Rect tmpRect = button->rect;

        tmpRect.x = button->rect.x - button->rect.w / 2;
        tmpRect.y = button->rect.y - button->rect.h / 2;

        if(button->bgTexture != NULL)
        {
            SDL_RenderCopy(rend,button->bgTexture,NULL,&tmpRect);
        }
        else
        {
            switch(button->state)
            {
            case ACTIVE:
                presentColor = button->highlightColor;
                break;

            case IDLE:
                presentColor = button->normalColor;
                break;

            case CLICKED:
                presentColor = ME_HexToSdlColor(0xff0000);
            }

            ME_RenderFillRect(rend, &tmpRect, presentColor);
        }

        MUI_RenderTextBox(&button->label, rend, MUI_TEXT_BLENDED);
    }

}

void MUI_DestroyButton(MUI_Button *button)
{
    SDL_DestroyTexture(button->bgTexture);
    button->bgTexture = NULL;

    MUI_DestroyTextBox(&(button->label));
}
