#include "ME_Rect.h"

ME_Rect ME_RectInit(i32 x, i32 y, u32 width, u32 height)
{
    ME_Rect rect = {0};
    
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    return rect;
}

ME_Rectf32 ME_Rectf32Init(f32 x, f32 y, f32 width, f32 height)
{
    ME_Rectf32 rect;
    
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    return rect;
}