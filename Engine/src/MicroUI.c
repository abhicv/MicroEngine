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
        ui->textInputChar = input->textInputChar;
        ui->bTextInput = input->bTextInput;
        ui->backSpaceDown = input->backSpaceDown;
    }
}

void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer)
{
    SDL_Rect rect = {0};
    SDL_Color color = {255, 255, 255, 255};
    SDL_Color textColor = {255, 255, 255, 255};
    
    TTF_Font *font = 0;
    SDL_Surface *surface = 0;
    SDL_Texture *tex = 0;
    
    bool highlighted = false;
    
    for (u32 i = 0; i < ui->widgetCount; i++)
    {
        switch (ui->widgets[i].widgetType)
        {
			case MUI_WIDGET_BUTTON:
            {
                highlighted = MUI_IdEqual(ui->hotWidgetId, ui->widgets[i].id);
                color.r = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.r : ui->widgets[i].style.buttonStyle.idleColor.r;
                color.g = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.g : ui->widgets[i].style.buttonStyle.idleColor.g;
                color.b = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.b : ui->widgets[i].style.buttonStyle.idleColor.b;
                
                rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
                ME_RenderFillRect(renderer, &rect, color);
                
                if(highlighted && ui->widgets[i].style.buttonStyle.enableHighlightBorder)
                {
                    SDL_Color borderColor = {255, 255, 255, 255};
                    ME_RenderDrawRect(renderer, &rect, borderColor);
                }
            }
            break;
            
            case MUI_WIDGET_SLIDER:
            {
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
            }
            break;
            
            case MUI_WIDGET_TEXTEDIT:
            {
                highlighted = MUI_IdEqual(ui->activeWidgetId, ui->widgets[i].id);
                color.r = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.r : ui->widgets[i].style.buttonStyle.idleColor.r;
                color.g = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.g : ui->widgets[i].style.buttonStyle.idleColor.g;
                color.b = highlighted ? ui->widgets[i].style.buttonStyle.highlightColor.b : ui->widgets[i].style.buttonStyle.idleColor.b;
                
                rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
                ME_RenderDrawRect(renderer, &rect, color);
                /*
                if(highlighted)
                {
                    SDL_Color borderColor = {255, 255, 255, 255};
                    ME_RenderDrawRect(renderer, &rect, borderColor);
                }
                */
                if(ui->fontFile != NULL)
                {
                    font = TTF_OpenFont(ui->fontFile, ui->widgets[i].style.buttonStyle.fontSize);
                }
                
                int fontHeight = 0;
                if(font != NULL)
                {
                    surface = TTF_RenderText_Blended(font, ui->widgets[i].text.text, textColor);
                    fontHeight = TTF_FontHeight(font);
                }
                
                if(surface != NULL)
                {
                    tex = SDL_CreateTextureFromSurface(renderer, surface);
                }
                
                SDL_Rect tmpRect = rect;
                tmpRect.w = 0;
                tmpRect.h = fontHeight;
                
                SDL_QueryTexture(tex, NULL, NULL, &tmpRect.w, &tmpRect.h);
                
                tmpRect.x = rect.x + 5;
                tmpRect.y = rect.y + (rect.h - tmpRect.h) / 2;
                
                if (tex != NULL && renderer != NULL)
                {
                    SDL_RenderCopy(renderer, tex, NULL, &tmpRect);
                }
                
                if(highlighted)
                {
                    SDL_Rect cursorRect;
                    cursorRect.x = tmpRect.x + tmpRect.w;
                    cursorRect.y = tmpRect.y;
                    cursorRect.w = 5;
                    cursorRect.h = tmpRect.h;
                    ME_RenderFillRect(renderer, &cursorRect, textColor);
                }
                
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surface);
                TTF_CloseFont(font);
            }
            break;
            
            case MUI_WIDGET_TEXT:
            {
                textColor.r = ui->widgets[i].style.textStyle.textColor.r;
                textColor.g = ui->widgets[i].style.textStyle.textColor.g;
                textColor.b = ui->widgets[i].style.textStyle.textColor.b;
                
                if(ui->fontFile != NULL)
                {
                    font = TTF_OpenFont(ui->fontFile, ui->widgets[i].text.fontSize);
                }
                
                if(font != NULL)
                {
                    surface = TTF_RenderText_Blended(font, ui->widgets[i].text.text, textColor);
                }
                
                if(surface != NULL)
                {
                    tex = SDL_CreateTextureFromSurface(renderer, surface);
                }
                
                rect = MUI_RectToSDL_Rect(&ui->widgets[i].rect);
                
                SDL_Rect tmpRect = rect;
                SDL_QueryTexture(tex, NULL, NULL, &tmpRect.w, &tmpRect.h);
                
                if(ui->widgets[i].text.alignType == MUI_TEXT_ALIGN_MIDDLE)
                {
                    tmpRect.x = rect.x + (rect.w - tmpRect.w) / 2;
                }
                else if(ui->widgets[i].text.alignType == MUI_TEXT_ALIGN_LEFT)
                {
                    tmpRect.x = rect.x + 5;
                }
                else if(ui->widgets[i].text.alignType == MUI_TEXT_ALIGN_RIGHT)
                {
                    tmpRect.x = (rect.x + rect.w) - (tmpRect.w + 5);
                }
                
                tmpRect.y = rect.y + (rect.h - tmpRect.h) / 2;
                
                if (tex != NULL && renderer != NULL)
                {
                    SDL_RenderCopy(renderer, tex, NULL, &tmpRect);
                    // ME_RenderDrawRect(renderer, &tmpRect, (SDL_Color){255, 255, 255 ,255});
                    // ME_RenderDrawRect(renderer, &rect, (SDL_Color){255, 0, 255 ,255});
                }
                
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surface);
                TTF_CloseFont(font);
            }
            break;
            
            default:
            break;
        }
    }
}

void MUI_Text(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize, MUI_Style style, u32 alignment)
{
    if(ui->widgetCount < MUI_MAX_WIDGETS)
    {
        MUI_Widget *widget = ui->widgets + ui->widgetCount++;
        widget->widgetType = MUI_WIDGET_TEXT;
        widget->id = id;
        widget->text.text = text;
        widget->text.fontSize = fontSize;
        widget->text.alignType = alignment;
        widget->rect = rect;
        widget->style = style;
    }
    else
    {
		printf("UI widget count out of bound\n");
    }
}

void MUI_TextA(MUI *ui, MUI_Id id, char *text, u32 fontSize, MUI_Style style, u32 alignment)
{
    MUI_Text(ui, id, MUI_GetNextAutoLayoutRect(ui), text, fontSize, style, alignment);
}

bool MUI_Button(MUI *ui, MUI_Id id, char *text, MUI_Rect rect, MUI_Style style)
{
    bool isTriggered = false;
    
    u32 left = rect.x;
    u32 right = rect.x + rect.width;
    u32 top = rect.y;
    u32 bottom = rect.y + rect.height;
    
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
    
    MUI_Text(ui, MUI_IdInit(id.primary - 10, id.secondary + 10), rect, text, style.buttonStyle.fontSize, textStyle, MUI_TEXT_ALIGN_MIDDLE);
    
    return isTriggered;
}

bool MUI_ButtonA(MUI *ui, MUI_Id id, char *text, MUI_Style style)
{
    return MUI_Button(ui, id, text, MUI_GetNextAutoLayoutRect(ui), style);
}

f32 MUI_Slider(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect, MUI_Style style)
{
    u32 left = rect.x;
    u32 right = rect.x + rect.width;
    u32 top = rect.y;
    u32 bottom = rect.y + rect.height;
    
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
            value = ((f32)ui->mouseX - (f32)left) / (f32)rect.width;
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
    else if (value > 1.0f)
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

void MUI_TextEdit(MUI *ui, MUI_Id id, MUI_Rect rect, MUI_Style style, TextEdit *textEdit)
{
    u32 left = rect.x;
    u32 right = rect.x + rect.width;
    u32 top = rect.y;
    u32 bottom = rect.y + rect.height;
    
    bool isMouseOver = (ui->mouseX > left && ui->mouseX < right &&
                        ui->mouseY > top && ui->mouseY < bottom);
    
    if (!MUI_IdEqual(id, ui->hotWidgetId) && isMouseOver)
    {
        ui->hotWidgetId = id;
    }
    else if (MUI_IdEqual(id, ui->hotWidgetId) && !isMouseOver && ui->leftMouseButtonDown)
    {
        ui->hotWidgetId = MUI_NullId();
        ui->activeWidgetId = MUI_NullId();
    }
    
    if (MUI_IdEqual(id, ui->activeWidgetId))
    {
        if (MUI_IdEqual(id, ui->hotWidgetId))
        {
            if(ui->bTextInput)
            {
                if(textEdit->cursorPos < MAX_TEXTEDIT_SIZE)
                {
                    textEdit->text[textEdit->cursorPos] = ui->textInputChar;
                    textEdit->cursorPos++;
                }
            }
            
            if(ui->backSpaceDown && textEdit->cursorPos > 0)
            {
                textEdit->cursorPos--;
                textEdit->text[textEdit->cursorPos] = 0;
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
        widget->widgetType = MUI_WIDGET_TEXTEDIT;
        widget->rect = rect;
        widget->style = style;
        widget->text.text = textEdit->text;
        widget->text.fontSize = 15;
    }
    else
    {
        printf("UI widget count out of bound\n");
    }
    
    /*
    MUI_Style textStyle = {
        .textStyle = {
            .textColor = (Color){255, 255, 255, 255},
        },
    };
    
    MUI_Text(ui, MUI_IdInit(id.primary - 10, id.secondary + 10), rect, textEdit->text, 15, textStyle);
    */
}

void MUI_TextEditA(MUI *ui, MUI_Id id, MUI_Style style, TextEdit *textEdit)
{
    MUI_TextEdit(ui, id, MUI_GetNextAutoLayoutRect(ui), style, textEdit);
}

SDL_Rect MUI_RectToSDL_Rect(MUI_Rect *rect)
{
    SDL_Rect sdlRect = {0};
    
    sdlRect.x = rect->x;
    sdlRect.y = rect->y;
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
    uiInput->bTextInput = false;
    
    SDL_StartTextInput();
    
    switch(event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            uiInput->leftMouseButtonDown = true;
        }
        else if (event->button.button == SDL_BUTTON_RIGHT)
        {
            uiInput->rightMouseButtonDown = true;
        }
        break;
        
        case SDL_MOUSEBUTTONUP:
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            uiInput->leftMouseButtonDown = false;
        }
        else if(event->button.button == SDL_BUTTON_RIGHT)
        {
            uiInput->rightMouseButtonDown = false;
        }
        break;
        
        case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_BACKSPACE)
        {
            uiInput->backSpaceDown = true;
        }
        break;
        
        case SDL_KEYUP:
        if (event->key.keysym.sym == SDLK_BACKSPACE)
        {
            uiInput->backSpaceDown = false;
        }
        break;
        
        case SDL_TEXTINPUT:
        uiInput->textInputChar = event->text.text[0];
        uiInput->bTextInput = true;
        break;

        case SDL_MOUSEMOTION:
        uiInput->mouseX = event->motion.x;
        uiInput->mouseY = event->motion.y;
        break;
    }
}

