#include "MUI_Button.h"
#include "ME_Utility.h"

const SDL_Color BUTTON_NORMAL_COLOR = {255, 255, 255, 255};
const SDL_Color BUTTON_ACTIVE_COLOR = {0, 0, 255, 255};

MUI_Button MUI_CreateButton(int x, int y)
{
    MUI_Button button;

    button.bgTexture = NULL;
    button.enabled = true;
    button.state = INACTIVE;
    button.rect.x = x;
    button.rect.y = y;
    button.rect.w = DEFAULT_BUTTON_WIDTH;
    button.rect.h = DEFAULT_BUTTON_HEIGHT;

    button.label = MUI_CreateTextBox(x, y, DEFAULT_BUTTON_FONT_SIZE);
    strcpy(button.label.textString, "Button");

    button.normalColor = BUTTON_NORMAL_COLOR;
    button.highlightColor = BUTTON_ACTIVE_COLOR;

    return button;
}

bool MUI_ButtonPressed(MUI_Button *button, SDL_Event event)
{
    SDL_Point mousePos;

    if (button->enabled)
    {
        SDL_Rect tmpRect = button->rect;

        mousePos.x = event.motion.x;
        mousePos.y = event.motion.y;
        tmpRect.x = button->rect.x - button->rect.w / 2;
        tmpRect.y = button->rect.y - button->rect.h / 2;

        if (SDL_PointInRect(&mousePos, &tmpRect))
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
            button->state = INACTIVE;
        }
    }

    return false;
}

void MUI_SetButtonPosition(MUI_Button *button, int x, int y)
{
    button->rect.x = x;
    button->rect.y = y;
}

void MUI_SetButtonLabel(MUI_Button *button, const char *label)
{
    if (label != NULL)
        strcpy(button->label.textString, label);
}

void MUI_SetButtonLabelFont(MUI_Button *button, TTF_Font *font)
{
    MUI_SetTextBoxFont(&button->label, font);
}

void MUI_RenderButton(MUI_Button *button, SDL_Renderer *renderer)
{
    SDL_Color buttonColor;

    if (button->enabled)
    {
        SDL_Rect tmpRect = button->rect;

        tmpRect.x = button->rect.x - button->rect.w / 2;
        tmpRect.y = button->rect.y - button->rect.h / 2;

        if (button->bgTexture != NULL)
        {
            SDL_RenderCopy(renderer, button->bgTexture, NULL, &tmpRect);
        }
        else
        {
            switch (button->state)
            {
            case ACTIVE:
                buttonColor = button->highlightColor;
                break;

            case INACTIVE:
                buttonColor = button->normalColor;
                break;

            case CLICKED:
                buttonColor = ME_HexToSdlColor(0xff0000);
            }

            ME_RenderFillRect(renderer, &tmpRect, buttonColor);
        }

        MUI_RenderTextBox(&button->label, renderer, MUI_TEXT_BLENDED);
    }
}

void MUI_DestroyButton(MUI_Button *button)
{
    SDL_DestroyTexture(button->bgTexture);
    button->bgTexture = NULL;

    MUI_DestroyTextBox(&(button->label));
}
