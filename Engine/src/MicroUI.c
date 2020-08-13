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
			case MUI_WIDGET_button:
            
            color.r = -20 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 70 - (!!MUI_IdEqual(ui->activeWidgetId, ui->widgets[i].id)) * 20;
            color.g = color.r;
            color.b = color.r;
            
            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            ME_RenderFillRect(renderer, &rect, color);
            
            if(!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id))
            {
                SDL_Color borderColor = {255, 255, 255, 255};
                ME_RenderDrawRect(renderer, &rect, borderColor);
            }
            
            break;
            
			case MUI_WIDGET_slider:
            
            //bg rect
            color.r = 255;
            color.g = 255;
            color.b = 255;
            
            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            ME_RenderFillRect(renderer, &rect, color);
            
            //sliding rect
            color.r = -10 * (!!MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id)) + 200;
            color.g = 0;
            color.b = 0;
            
            SDL_Rect slideRect = rect;
            slideRect.w = (f32)rect.w * ui->widgets[i].slider.value;
            ME_RenderFillRect(renderer, &slideRect, color);
            
            break;
            
			case MUI_Widget_text:
            
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
            
            case MUI_Widget_textbox:
            
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
            
            SDL_Rect textRect = {0};
            SDL_QueryTexture(tex, NULL, NULL, &textRect.w, &textRect.h);
            
            rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
            
            textRect.x = rect.x + 4;
            textRect.y = rect.y + 4;
            
            rect.w = textRect.w + 4;
            rect.h = textRect.h + 4;
            
            ME_RenderFillRect(renderer, &rect, color);
            
            if (tex != NULL && renderer != NULL)
            {
                SDL_RenderCopy(renderer, tex, NULL, &textRect);
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

void MUI_Text(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize)
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

void MUI_TextA(MUI *ui, MUI_Id id, char *text, u32 fontSize)
{
    MUI_Text(ui, id, MUI_GetNextAutoLayoutRect(ui), text, fontSize);
}

void MUI_TextBox(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize)
{
    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->widgetType = MUI_Widget_textbox;
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

bool MUI_Button(MUI *ui, MUI_Id id, char *text, MUI_Rect rect)
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
        widget->widgetType = MUI_WIDGET_button;
        widget->rect = rect;
    }
    else
    {
        printf("UI widget count out of bound\n");
    }
    
    MUI_Text(ui, MUI_IdInit(id.primary - 10, id.secondary + 10), rect, text, 14);
    
    return isTriggered;
}

bool MUI_ButtonA(MUI *ui, MUI_Id id, char *text)
{
    return MUI_Button(ui, id, text, MUI_GetNextAutoLayoutRect(ui));
}

f32 MUI_Slider(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect)
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

f32 MUI_SliderA(MUI *ui, MUI_Id id, f32 value)
{
    return MUI_Slider(ui, id, value, MUI_GetNextAutoLayoutRect(ui));
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