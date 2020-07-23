#ifndef MICROPHYSICS_H
#define MICROPHYSICS_H

#include <SDL2/SDL.h>

#include "ME_Vector2D.h"
#include "ME_Rect.h"

typedef ME_Rectf32 CollisionRect;

typedef struct CollisionInfo
{
    Vector2 normal;
    f32 penetrationDepth;
    bool collided;

} CollisionInfo;

typedef struct PhysicsBody
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 force;

    f32 mass;
    f32 inverseMass;
    
    CollisionRect rect;
    f32 restitution;
    f32 friction;
    
    bool affectedByGravity;

} PhysicsBody;

PhysicsBody CreatePhysicsBody(Vector2 position, f32 mass, f32 width, f32 height);
void UpdatePhysics(PhysicsBody *body, Vector2 gravity, f32 deltaTime);
void ResolveCollision(PhysicsBody *a, PhysicsBody *b, CollisionInfo info);
CollisionInfo DetectCollision(CollisionRect *a, CollisionRect *b);

#endif // MICROPHYSICS_H
