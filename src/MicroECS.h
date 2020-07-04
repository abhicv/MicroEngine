#ifndef MICROECS_H
#define MICROECS_H

//TODO(Lone-code) : Implement a entity component system
#include "types.h"
#include "ME_Vector2D.h"

typedef struct
{
    Vector2 position;
    Vector2 size;
    f32 angle;

} TransformCompnent;

typedef struct
{

} SpriteComponent;

typedef struct
{
    TransformCompnent *transforms;
    SpriteComponent *sprites;

} MicroECSWorld;

u32 CreateEntity();
#endif