#ifndef MICROECS_H
#define MICROECS_H

//TODO(Lone-code) : Implement a entity component system
#include "types.h"

typedef struct 
{

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