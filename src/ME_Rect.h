#ifndef ME_RECT_H
#define ME_RECT_H 

#include "types.h"

typedef struct
{
    i32 x;
    i32 y;
    u32 width;
    u32 height;

} ME_Rect;

typedef struct
{
    f32 x;
    f32 y;
    f32 width;
    f32 height;

} ME_Rectf32;

ME_Rect ME_RectInit(i32 x, i32 y, u32 width, u32 height);
ME_Rectf32 ME_Rectf32Init(f32 x, f32 y, f32 width, f32 height);

#endif