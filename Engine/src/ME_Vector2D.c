#include "ME_Vector2D.h"

Vector2 Vector2Init(float x, float y) {
    Vector2 v;
    v.x = x;
    v.y = y;
    return v;
}

Vector2 Vector2Null()
{
    return Vector2Init(0.0f, 0.0f);
}

float Vector2Mag(Vector2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

float Vector2SqrMag(Vector2 v) {
    return (v.x * v.x + v.y * v.y);
}

float Vector2Dot(Vector2 v1, Vector2 v2) {
    return (v1.x * v2.x + v1.y * v2.y);
}

void Vector2Normalize(Vector2 *v) {
    f32 mag = Vector2Mag(*v);
    v->x /= mag;
    v->y /= mag;
}

void Vector2Scale(Vector2 *v, f32 scalaFactor) {
    v->x *= scalaFactor;
    v->y *= scalaFactor;
}

Vector2 Vector2ScalarMultiply(Vector2 v, f32 scalar) 
{
    Vector2 result;
    
    result.x = v.x * scalar;
    result.y = v.y * scalar;

    return result;
}

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) {
    Vector2 v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    return v;
}

Vector2 ReflectDir(Vector2 incident, Vector2 normal) {
    float dot = 2 * Vector2Dot(incident, normal);
    Vector2Scale(&normal, dot);
    return Vector2Subtract(incident, normal);
}
