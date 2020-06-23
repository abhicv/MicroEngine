#include "MicroUI.h"

#include "MUI_Button.c"
#include "MUI_CheckBox.c"
#include "MUI_TextBox.c"
#include "MUI_Slider.c"

internal bool MUI_IdEqual(MUI_Id a, MUI_Id b)
{
    return (a.primary == b.primary && a.secondary == b.secondary);
}

internal MUI_Id MUI_NullId()
{
    MUI_Id id = {0, 0};
    return id;
}

internal void MUI_BeginFrame(MUI *ui, MUI_Input *input)
{
    ui->widgetCount = 0;

    ui->leftMouseButtonDown = input->leftMouseButtonDown;
    ui->rightMouseButtonDown = input->rightMouseButtonDown;
    ui->mouseX = input->mouseX;
    ui->mouseY = input->mouseY;
}

internal void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer)
{
    int i = 0;
    for (i = 0; i < ui->widgetCount; i++)
    {
        switch (ui->widgets[i].widgetType)
        {
        case MUI_WIDGET_button:

            u8 r = 150 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 105 - (!!MUI_IdEqual(ui->activeWidgetId, ui->widgets[i].id)) * 40;
            u8 g = r, b = r;

            SDL_Rect rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            SDL_SetRenderDrawColor(renderer, r, g, b, 0);
            SDL_RenderFillRect(renderer, &rect);
            break;

        case MUI_WIDGET_slider:
            // u8 rS = 150 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 105 - (!!MUI_IdEqual(ui->activeWidgetId, ui->widgets[i].id)) * 40;
            // u8 gS = rS, bS = rS;

            SDL_Rect rectS = MUI_RectToSDL_Rect(&ui->widgets[i].rect);

            SDL_Rect slideRect = rectS;
            slideRect.w = (f32)rectS.w * ui->widgets[i].slider.value;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            SDL_RenderFillRect(renderer, &rectS);

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0);
            SDL_RenderFillRect(renderer, &slideRect);

            break;
        default:
            break;
        }
    }
}

internal bool MUI_ButtonP(MUI *ui, MUI_Id id, char *text, MUI_Rect rect)
{
    MUI_Widget *widget = ui->widgets + ui->widgetCount++;

    bool isTriggered = false;

    widget->id = id;
    widget->widgetType = MUI_WIDGET_button;
    widget->rect = rect;

    u32 left = rect.x - rect.width / 2;
    u32 right = rect.x + rect.width / 2;
    u32 top = rect.y - rect.height / 2;
    u32 bottom = rect.y + rect.height / 2;

    bool isMouseOver = (ui->mouseX > left && ui->mouseX < right && ui->mouseY > top && ui->mouseY < bottom);

    if (!MUI_IdEqual(id, ui->hotWidgetId) && isMouseOver)
    {
        ui->hotWidgetId = id;
    }
    else if (MUI_IdEqual(id, ui->hotWidgetId) && !isMouseOver)
    {
        ui->hotWidgetId = MUI_NullId();
    }

    if (MUI_IdEqual(id, ui->activeWidgetId))
    {
        if (!ui->leftMouseButtonDown)
        {
            isTriggered = MUI_IdEqual(id, ui->hotWidgetId);
            ui->activeWidgetId = MUI_NullId();
        }
    }
    else
    {
        if (MUI_IdEqual(id, ui->hotWidgetId))
        {
            if (ui->leftMouseButtonDown)
            {
                ui->activeWidgetId = id;
            }
        }
    }

    return isTriggered;
}

internal f32 MUI_SliderP(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect)
{
    u32 left = rect.x - rect.width / 2;
    u32 right = rect.x + rect.width / 2;
    u32 top = rect.y - rect.height / 2;
    u32 bottom = rect.y + rect.height / 2;

    bool isMouseOver = (ui->mouseX > left && ui->mouseX < right && ui->mouseY > top && ui->mouseY < bottom);

    if (!MUI_IdEqual(id, ui->hotWidgetId) && isMouseOver)
    {
        ui->hotWidgetId = id;
    }
    else if (MUI_IdEqual(id, ui->hotWidgetId) && !isMouseOver)
    {
        ui->hotWidgetId = MUI_NullId();
    }

    if (!MUI_IdEqual(id, ui->activeWidgetId))
    {
        if (MUI_IdEqual(id, ui->hotWidgetId))
        {
            if (ui->leftMouseButtonDown)
            {
                ui->activeWidgetId = id;
            }
        }
    }

    if (MUI_IdEqual(id, ui->activeWidgetId))
    {
        if (ui->leftMouseButtonDown)
        {
            value = (f32)(ui->mouseX - left) / (f32)rect.width;
        }
        else
        {
            ui->activeWidgetId = MUI_NullId();
        }
    }

    if (value < 0.001f)
    {
        value = 0.0f;
    }
    if (value > 1.0f)
    {
        value = 1.0f;
    }

    MUI_Widget *widget = ui->widgets + ui->widgetCount++;
    widget->id = id;
    widget->widgetType = MUI_WIDGET_slider;
    widget->rect = rect;
    widget->slider.value = value;

    return value;
}

SDL_Rect MUI_RectToSDL_Rect(MUI_Rect *rect)
{
    SDL_Rect sdlRect = {0};

    sdlRect.x = rect->x - rect->width / 2;
    sdlRect.y = rect->y - rect->height / 2;
    sdlRect.w = rect->width;
    sdlRect.h = rect->height;

    return sdlRect;
}
