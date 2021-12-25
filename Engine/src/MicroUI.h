#ifndef MICROUI_H
#define MICROUI_H

#include "ME_Rect.h"
typedef ME_Rect MUI_Rect;

#define MUI_MAX_WIDGETS 128
#define MUI_MAX_AUTOLAYOUT_SIZE 16

#define BIT_5x3_FONT_FILE "assets/Font/bit5x3.ttf"
#define OPENSANS_FONT_FILE "assets/Font/OpenSans-Regular.ttf"
#define AGOESTOESAN_FONT_FILE "assets/Font/agoestoesan.ttf"

enum
{
    MUI_WIDGET_BUTTON,
    MUI_WIDGET_SLIDER,
    MUI_WIDGET_TEXT,
    MUI_WIDGET_ICON_TEXT,
    MUI_WIDGET_COUNT,
    MUI_WIDGET_TEXTEDIT,
};

enum 
{
    MUI_TEXT_ALIGN_LEFT = 0,
    MUI_TEXT_ALIGN_MIDDLE = 1,
    MUI_TEXT_ALIGN_RIGHT = 2,
};

typedef struct Color
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    
} Color;

typedef struct MUI_Style
{
    union
    {
        struct ButtonStyle
        {
            Color idleColor;
            Color highlightColor;
            Color textColor;
            u32 fontSize;
            
        } buttonStyle;
        
        struct SliderStyle
        {
            Color sliderColor;
            Color bgColor;
            
        } sliderStyle;
        
        struct TextStyle
        {
            Color textColor;
            
        } textStyle;
    };
    
} MUI_Style;

typedef struct MUI_Id
{
    u32 primary;
    u32 secondary;
    
} MUI_Id;

#define MAX_TEXTEDIT_SIZE 250

typedef struct TextEdit
{
    u8 text[MAX_TEXTEDIT_SIZE];
    u32 cursorPos;
    
} TextEdit;

typedef struct MUI_Widget
{
    u32 widgetType;
    MUI_Id id;
    MUI_Rect rect;
    MUI_Style style;
    
    union 
    {
        struct Slider
        {
            f32 value;
            
        } slider;
        
        struct Text
        {
            char *text;
            u32 fontSize;
            u32 alignType;
        } text;
    };
    
} MUI_Widget;

typedef struct MUI_Input
{
    u8 textInputChar;
    
    i32 mouseX;
    i32 mouseY;
    
    b32 leftMouseButtonDown;
    b32 rightMouseButtonDown;
    b32 backSpaceDown;
    b32 bTextInput;
    
} MUI_Input;

typedef struct MUI
{
    u8 textInputChar;
    
    i32 mouseX;
    i32 mouseY;
    
    b32 leftMouseButtonDown;
    b32 rightMouseButtonDown;
    b32 backSpaceDown;
    b32 bTextInput;
    
    u32 widgetCount;
    
    MUI_Id hotWidgetId;
    MUI_Id activeWidgetId;
    
    MUI_Widget widgets[MUI_MAX_WIDGETS];
    
    u32 autoLayOutIndex;
    struct AutoLayOutGroup
    {
        MUI_Rect rect;
        u32 progress;
        bool isColumn;
        u32 offset;
        
    } autoLayOutGroup[MUI_MAX_AUTOLAYOUT_SIZE];
    
    char *fontFile;
    
} MUI;

void MUI_BeginFrame(MUI *ui, MUI_Input *input);
void MUI_EndFrame(MUI *ui, SDL_Renderer *renderer);
void MUI_GetInput(MUI_Input *uiInput, SDL_Event *event);

MUI_Id MUI_IdInit(u32 primary, u32 secondary);
bool MUI_IdEqual(MUI_Id a, MUI_Id b);
MUI_Id MUI_NullId(void);

void MUI_Text(MUI *ui, MUI_Id id, MUI_Rect rect, char *text, u32 fontSize, MUI_Style style, u32 alignment);
void MUI_TextA(MUI *ui, MUI_Id id, char *text, u32 fontSize, MUI_Style style, u32 alignment);

bool MUI_Button(MUI *ui, MUI_Id id, char *text, MUI_Rect rect, MUI_Style style);
bool MUI_ButtonA(MUI *ui, MUI_Id id, char *text, MUI_Style style);

f32 MUI_Slider(MUI *ui, MUI_Id id, f32 value, MUI_Rect rect, MUI_Style style);
f32 MUI_SliderA(MUI *ui, MUI_Id id, f32 value, MUI_Style style);

void MUI_PushColumnLayout(MUI *ui, MUI_Rect rect, u32 offset);
void MUI_PushRowLayout(MUI *ui, MUI_Rect rect, u32 offset);
void MUI_PopLayout(MUI *ui);
MUI_Rect MUI_GetNextAutoLayoutRect(MUI *ui);

SDL_Rect MUI_RectToSDL_Rect(MUI_Rect *rect);

#define GEN_MUI_ID() MUI_IdInit((u32)(__LINE__), (u32)MUI_ORIGIN_ID)
#define GEN_MUI_IDi(i) MUI_IdInit((u32)(__LINE__ + i), (u32)MUI_ORIGIN_ID)

#endif //MICROUI_H