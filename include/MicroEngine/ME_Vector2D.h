#ifndef ME_VECTOR2D_H
#define ME_VECTOR2D_H

#include <math.h>

typedef struct
{
    float x,y;

} Vector2;

Vector2 NewVector2(float,float);
float Vector2Mag(Vector2);
float Vector2SqrMag(Vector2);
float Vector2Dot(Vector2,Vector2);
void Vector2Normalize(Vector2*);
void Vector2Scale(Vector2*,float);
Vector2 Vector2Subtract(Vector2,Vector2);
Vector2 ReflectDir(Vector2,Vector2);

#endif //ME_VECTOR2D_H
