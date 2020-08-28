#include "MicroUI.h"

MUI_Id MUI_IdInit(u32 primary, u32 secondary)
{
    MUI_Id id = {primary, secondary};
    return id;
}

bool MUI_IdEqual(MUI_Id a, MUI_Id b)
{
    return (a.primary == b.primary && a.secondary == b.secondary);
}

MUI_Id MUI_NullId(void)
{
    MUI_Id id = {0, 0};
    return id;
}

void MUI_BeginFrame(MUI *ui, MUI_Input *input)
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

void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer)
{
    SDL_Rect rect = {0};
    SDL_Color color = {255, 255, 255, 255};
    SDL_Color textColor = {255, 255, 255, 255};
    
    TTF_Font *font = NULL;
    SDL_Surface *surface = NULL;
    SDL_Texture *tex = NULL;
    
    u32 i = 0;
    for (i = 0; i < ui->widgetCount; i++)
    {
        switch (ui->widgets[i].widgetType)
        {
			case MUI_WIDGET_BUTTON:
            
            bool highlighted = MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id);
            color.r = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.r : ui->widgets[i].style.buttonStyle.idleColor.r;
            color.g = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.g : ui->widgets[i].style.buttonStyle.idleColor.g;
            color.b = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.b : ui->widgets[i].style.buttonStyle.idleColor.b;
            
            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            ME_RenderFillRect(renderer, &rect, color);
            
            if(MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id))
            {
                SDL_Color borderColor = {255, 255, 255, 255};
                ME_RenderDrawRect(renderer, &rect, borderColor);
            }
            
            break;
            
            case MUI_WIDGET_SLIDER:
            
            //bg rect
            color.r = ui->widgets[i].style.sliderStyle.bgColor.r;
            color.g = ui->widgets[i].style.sliderStyle.bgColor.g;
            color.b = ui->widgets[i].style.sliderStyle.bgColor.b;
            
            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            ME_RenderFillRect(renderer, &rect, color);
            
            //sliding rect
            color.r = ui->widgets[i].style.sliderStyle.sliderColor.r;
            color.g = ui->widgets[i].style.sliderStyle.sliderColor.g;
            color.b = ui->widgets[i].style.sliderStyle.sliderColor.b;
            
            SDL_Rect slideRect = rect;
            slideRect.w = (f32)rect.w * ui->widgets[i].slider.value;
            ME_RenderFillRect(renderer, &slideRect, color);
            
            break;
            
            case MUI_WIDGET_TEXT:
            
            textColor.r = ui->widgets[i].style.textStyle.textColor.r;
            textColor.g = ui->widgets[i].style.textStyle.textColor.g;
            textColor.b = ui->widgets[i].style.textStyle.textColor.b;
            
            if (ui->fontFile != NULL)
            {
                font = TTF_OpenFont(ui->fontFile, ui->widgets[i].text.fontSize);
            }
            if (font != NULL)
            {
                surface = TTF_RenderText_Blended(font, ui->widgets[i].text.text, textColor);
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

void MUI_Text(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize, MUI_Style style)
{
    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->widgetType = MUI_WIDGET_TEXT;
        widget->id = id;
        widget->text.text = text;
        widget->text.fontSize = fontSize;
        widget->rect = rect;
        widget->style = style;
    }
    else
    {
		printf("UI widget count out of bound\n");
    }
}

void MUI_TextA(MUI *ui, MUI_Id id, char *text, u32 fontSize, MUI_Style style)
{
    MUI_Text(ui, id, MUI_GetNextAutoLayoutRect(ui), text, fontSize, style);
}

bool MUI_Button(MUI *ui, MUI_Id id, char *text, MUI_Rect rect, MUI_Style style)
{
    bool isTriggered = false;
    
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
        widget->widgetType = MUI_WIDGET_BUTTON;
        widget->rect = rect;
        widget->style = style;
    }
    else
    {
        printf("MUI widget count out of bound\n");
    }
    
    MUI_Style textStyle = {
        .textStyle = {
            .textColor = style.buttonStyle.textColor,
        },
    };
    
    MUI_Text(ui, MUI_IdInit(id.primary - 10, id.secondary + 10), rect, text, style.buttonStyle.fontSize, textStyle);
    
    return isTriggered;
}

bool MUI_ButtonA(MUI *ui, MUI_Id id, char *text, MUI_Style style)
{
    return MUI_Button(ui, id, text, MUI_GetNextAutoLayoutRect(ui), style);
}

f32 MUI_Slider(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect, MUI_Style style)
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
        widget->widgetType = MUI_WIDGET_SLIDER;
        widget->rect = rect;
        widget->slider.value = value;
        widget->style = style;
    }
    else
    {
        printf("UI widget count out of bound\n");
    }
    
    return value;
}

f32 MUI_SliderA(MUI *ui, MUI_Id id, f32 value, MUI_Style style)
{
    return MUI_Slider(ui, id, value, MUI_GetNextAutoLayoutRect(ui), style);
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

void MUI_PushColumnLayout(MUI *ui, MUI_Rect rect, u32 offset)
{
    u32 i = ui->autoLayOutIndex++;
    ui->autoLayOutGroup[i].rect = rect;
    ui->autoLayOutGroup[i].progress = 0;
    ui->autoLayOutGroup[i].offset = offset;
    ui->autoLayOutGroup[i].isColumn = true;
}

void MUI_PushRowLayout(MUI *ui, MUI_Rect rect, u32 offset)
{
    u32 i = ui->autoLayOutIndex++;
    ui->autoLayOutGroup[i].rect = rect;
    ui->autoLayOutGroup[i].progress = 0;
    ui->autoLayOutGroup[i].offset = offset;
    ui->autoLayOutGroup[i].isColumn = false;
}

void MUI_PopLayout(MUI *ui)
{
    if (ui->autoLayOutIndex > 0)
    {
        ui->autoLayOutIndex--;
    }
}

MUI_Rect MUI_GetNextAutoLayoutRect(MUI *ui)
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

void MUI_GetInput(MUI_Input *uiInput, SDL_Event *event)
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