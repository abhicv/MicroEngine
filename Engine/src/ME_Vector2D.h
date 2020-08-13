#ifndef ME_VECTOR2D_H
#define ME_VECTOR2D_H

#include <math.h>
#include "types.h"

#define PI 3.141592653589f

typedef struct Vector2
{
    f32 x;
    f32 y;
    
} Vector2;

typedef Vector2 Vec2;
typedef Vector2 v2;

Vector2 Vector2Init(f32 x, f32 y);
Vector2 Vector2Null();

float Vector2Mag(Vector2 v);
float Vector2SqrMag(Vector2 v);
float Vector2Dot(Vector2, Vector2 v);

void Vector2Normalize(Vector2 *v);
void Vector2Scale(Vector2 *v, f32 scalar);

Vector2 Vector2ScalarMultiply(Vector2 v, f32 scalar); 
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);
Vector2 ReflectDir(Vector2 v1, Vector2 v2);

#endif //ME_VECTOR2D_H
