#ifndef MICROUI_H
#define MICROUI_H

#include "MUI_Button.h"
#include "MUI_TextBox.h"
#include "MUI_CheckBox.h"
#include "MUI_Slider.h"

#define MUI_MAX_WIDGETS 128

enum
{
    MUI_WIDGET_button,
    MUI_WIDGET_slider,
};

typedef struct
{
    i32 x;
    i32 y;
    u32 width;
    u32 height;

} MUI_Rect;

typedef struct
{
    u32 primary;
    u32 secondary;

} MUI_Id;

typedef struct
{
    u32 widgetType;
    MUI_Id id;
    MUI_Rect rect;

    union {
        struct Slider
        {
            f32 value;

        } slider;
    };

} MUI_Widget;

typedef struct
{
    i32 mouseX;
    i32 mouseY;

    bool leftMouseButtonDown;
    bool rightMouseButtonDown;

} MUI_Input;

typedef struct
{
    i32 mouseX;
    i32 mouseY;
    bool leftMouseButtonDown;
    bool rightMouseButtonDown;

    int widgetCount;
    MUI_Id hotWidgetId;
    MUI_Id activeWidgetId;

    MUI_Widget widgets[MUI_MAX_WIDGETS];

} MUI;

void MUI_BeginFrame(MUI *ui);
void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer);

bool MUI_IdEqual(MUI_Id a, MUI_Id b);
MUI_Id MUI_NullId();

bool MUI_ButtonP(MUI *ui, MUI_Id id, char *text, MUI_Rect rect);
f32 MUI_SliderP(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect);
//NOTE(Lonecoder) : Depends on sdl rect
SDL_Rect MUI_RectToSDL_Rect(MUI_Rect *rect);
#endif //MICROUI_H