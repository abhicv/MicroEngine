#include "MicroUI.h"

#include "MUI_Button.c"
#include "MUI_CheckBox.c"
#include "MUI_TextBox.c"
#include "MUI_Slider.c"

internal MUI_Id MUI_IdInit(u32 primary, u32 secondary)
{
    MUI_Id id = {primary, secondary};
    return id;
}

internal bool MUI_IdEqual(MUI_Id a, MUI_Id b)
{
    return (a.primary == b.primary && a.secondary == b.secondary);
}

internal MUI_Id MUI_NullId(void)
{
    MUI_Id id = {0, 0};
    return id;
}

internal MUI_Rect MUI_RectInit(i32 x, i32 y, u32 w, u32 h)
{
    MUI_Rect rect = {x, y, w, h};
    return rect;
}

internal void MUI_BeginFrame(MUI *ui, MUI_Input *input)
{
    ui->widgetCount = 0;

    if (input != NULL)
    {
        ui->leftMouseButtonDown = input->leftMouseButtonDown;
        ui->rightMouseButtonDown = input->rightMouseButtonDown;
        ui->mouseX = input->mouseX;
        ui->mouseY = input->mouseY;
    }
}

internal void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer)
{
    u32 i = 0;

    SDL_Rect rect = {0};
    SDL_Color color = {0};

    for (i = 0; i < ui->widgetCount; i++)
    {
        switch (ui->widgets[i].widgetType)
        {
        case MUI_WIDGET_button:

            color.r = -20 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 50 - (!!MUI_IdEqual(ui->activeWidgetId, ui->widgets[i].id)) * 10;
            color.g = color.r;
            color.b = color.r;

            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            ME_RenderFillRect(renderer, &rect, color);
            
            break;

        case MUI_WIDGET_slider:
        
            color.r = -10 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 70;
            color.g = color.r;
            color.b = color.r;

            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);

            //slider rect
            SDL_Color c = color;
            c.r -= 20;
            c.g = c.b = c.r;
            ME_RenderFillRect(renderer, &rect, c);

            //sliding rect
            SDL_Rect slideRect = rect;
            slideRect.w = (f32)rect.w * ui->widgets[i].slider.value;
            ME_RenderFillRect(renderer, &slideRect, color);

            break;

        case MUI_Widget_text:

            SDL_Color textColor = {255, 255, 0, 255};

            TTF_Font *font = NULL;
            SDL_Surface *surface = NULL;
            SDL_Texture *tex = NULL;

            if (ui->fontFile != NULL)
            {
                font = TTF_OpenFont(ui->fontFile, ui->widgets[i].text.fontSize);
            }
            if (font != NULL)
            {
                surface = TTF_RenderText_Shaded(font, ui->widgets[i].text.text, textColor, ME_GetRenderColor(renderer));
            }
            if (surface != NULL)
            {
                tex = SDL_CreateTextureFromSurface(renderer, surface);
            }

            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);

            SDL_Rect tmpRect = rect;
            SDL_QueryTexture(tex, NULL, NULL, &tmpRect.w, &tmpRect.h);

            tmpRect.x = rect.x + (rect.w - tmpRect.w) / 2;
            tmpRect.y = rect.y + (rect.h - tmpRect.h) / 2;

            // ME_RenderDrawRect(renderer, &tmpRect, color);
            if (tex != NULL && renderer != NULL)
            {
                SDL_RenderCopy(renderer, tex, NULL, &tmpRect);
            }

            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surface);
            TTF_CloseFont(font);

            break;

        default:
            break;
        }
    }
}

internal MUI_TextP(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize)
{
    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->widgetType = MUI_Widget_text;
        widget->id = id;
        widget->text.text = text;
        widget->text.fontSize = fontSize;
        widget->rect = rect;
    }
    else
    {
         printf("UI widget count out of bound\n");
    }   
}

internal MUI_TextA(MUI *ui, MUI_Id id, char *text, u32 fontSize)
{
    MUI_TextP(ui, id, MUI_GetNextAutoLayoutRect(ui), text, fontSize);
}

internal bool MUI_ButtonP(MUI *ui, MUI_Id id, char *text, MUI_Rect rect)
{

    bool isTriggered = false;
    
    MUI_TextP(ui, MUI_IdInit(id.primary - 10, id.secondary + 10), rect, text, 15);
 
    u32 left = rect.x - rect.width / 2;
    u32 right = rect.x + rect.width / 2;
    u32 top = rect.y - rect.height / 2;
    u32 bottom = rect.y + rect.height / 2;

    bool isMouseOver = (ui->mouseX > left &&
                        ui->mouseX < right &&
                        ui->mouseY > top &&
                        ui->mouseY < bottom);

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
            if (MUI_IdEqual(id, ui->hotWidgetId))
            {
                isTriggered = true;
                ui->activeWidgetId = MUI_NullId();
            }
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

    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->id = id;
        widget->widgetType = MUI_WIDGET_button;
        widget->rect = rect; 
    }
    else
    {
        printf("UI widget count out of bound\n");
    }

    return isTriggered;
}

internal bool MUI_ButtonA(MUI *ui, MUI_Id id, char *text)
{
    return MUI_ButtonP(ui, id, text, MUI_GetNextAutoLayoutRect(ui));
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

    if (value < 0.0f)
    {
        value = 0.0f;
    }
    if (value > 1.0f)
    {
        value = 1.0f;
    }

    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->id = id;
        widget->widgetType = MUI_WIDGET_slider;
        widget->rect = rect;
        widget->slider.value = value;
    }
    else
    {
        printf("UI widget count out of bound\n");
    }
    
    return value;
}

internal f32 MUI_SliderA(MUI *ui, MUI_Id id, f32 value)
{
    return MUI_SliderP(ui, id, value, MUI_GetNextAutoLayoutRect(ui));
}

internal SDL_Rect MUI_RectToSDL_Rect(MUI_Rect *rect)
{
    SDL_Rect sdlRect = {0};

    sdlRect.x = rect->x - rect->width / 2;
    sdlRect.y = rect->y - rect->height / 2;
    sdlRect.w = rect->width;
    sdlRect.h = rect->height;

    return sdlRect;
}

internal void MUI_PushColumnLayout(MUI *ui, MUI_Rect rect, u32 offset)
{
    u32 i = ui->autoLayOutIndex++;
    ui->autoLayOutGroup[i].rect = rect;
    ui->autoLayOutGroup[i].progress = 0;
    ui->autoLayOutGroup[i].offset = offset;
    ui->autoLayOutGroup[i].isColumn = true;
}

internal void MUI_PushRowLayout(MUI *ui, MUI_Rect rect, u32 offset)
{
    u32 i = ui->autoLayOutIndex++;
    ui->autoLayOutGroup[i].rect = rect;
    ui->autoLayOutGroup[i].progress = 0;
    ui->autoLayOutGroup[i].offset = offset;
    ui->autoLayOutGroup[i].isColumn = false;
}

internal void MUI_PopLayout(MUI *ui)
{
    if (ui->autoLayOutIndex > 0)
    {
        ui->autoLayOutIndex--;
    }
}

internal MUI_Rect MUI_GetNextAutoLayoutRect(MUI *ui)
{
    MUI_Rect rect = {0};

    if (ui->autoLayOutIndex > 0)
    {
        u32 i = ui->autoLayOutIndex - 1;

        if (ui->autoLayOutGroup[i].isColumn)
        {
            rect = ui->autoLayOutGroup[i].rect;
            rect.y = ui->autoLayOutGroup[i].progress + rect.y;
            ui->autoLayOutGroup[i].progress += rect.height + ui->autoLayOutGroup[i].offset;
        }
        else
        {
            rect = ui->autoLayOutGroup[i].rect;
            rect.x = ui->autoLayOutGroup[i].progress + rect.x;
            ui->autoLayOutGroup[i].progress += rect.width + ui->autoLayOutGroup[i].offset;
        }
    }

    return rect;
}

internal void MUI_GetInput(MUI_Input *uiInput, SDL_Event *event)
{
    uiInput->mouseX = event->motion.x;
    uiInput->mouseY = event->motion.y;

    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            uiInput->leftMouseButtonDown = true;
        }
        else if (event->button.button == SDL_BUTTON_RIGHT)
        {
            uiInput->rightMouseButtonDown = true;
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            uiInput->leftMouseButtonDown = false;
        }
        else if (event->button.button == SDL_BUTTON_RIGHT)
        {
            uiInput->rightMouseButtonDown = false;
        }
    }
}