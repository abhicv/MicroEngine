#include "MicroPhysics.h"

f32 AbsoluteValueF32(f32 x)
{
    return (x >= 0 ? x : -x);
}

PhysicsBody CreatePhysicsBody(Vector2 position, f32 mass, f32 width, f32 height)
{
    f32 inverseMass = 0.0f;
    
    if(mass == 0.0f)
    {
        inverseMass = 0.0f;
    }
    else
    {
        inverseMass = 1.0f / mass;
    }
    
    CollisionRect rect = {position.x, position.y, width, height};
    
    PhysicsBody physicsBody =  {0};
    physicsBody.position = position;
    physicsBody.velocity = Vector2Null();
    physicsBody.acceleration = Vector2Null();
    physicsBody.force = Vector2Null();
    physicsBody.mass = mass;
    physicsBody.inverseMass = inverseMass;
    physicsBody.rect = rect;
    physicsBody.restitution = 0.0f;
    physicsBody.friction = 0.0f;
    physicsBody.affectedByGravity = true;
    
    return physicsBody;
}

void UpdatePhysics(PhysicsBody *body, Vector2 gravity, f32 deltaTime)
{
    if(body->affectedByGravity)
    {
        body->velocity.y += gravity.y * deltaTime;
    }
    
    body->acceleration.x = (body->force.x * body->inverseMass);
    body->acceleration.y = (body->force.y * body->inverseMass);
    
    body->velocity.x += (body->acceleration.x * deltaTime);
    body->velocity.y += (body->acceleration.y * deltaTime);
    
    //NOTE(abhicv): applying friction in x direction
    body->velocity.x -= (body->velocity.x * body->friction);
    
    body->position.x += body->velocity.x * deltaTime;
    body->position.y += body->velocity.y * deltaTime;
    
    body->force = Vector2Null();
    body->acceleration = Vector2Null();
}

void swap(f32 *a, f32 *b)
{
    f32 temp = *a;
    *a = *b;
    *b = temp;
}

bool DetectRayRectCollision(Vector2 origin, Vector2 dir, CollisionRect *target, f32 *t, Vector2 *contactPoint, Vector2 *contactNormal)
{
    f32 x = target->x - target->width / 2;
    f32 y = target->y - target->height / 2;
    
    f32 xNear = (x - origin.x) / dir.x;
    f32 xFar = (x + target->width  - origin.x) / dir.x;
    
    f32 yNear = (y - origin.y) / dir.y;
    f32 yFar = (y + target->height  - origin.y) / dir.y;
    
    if(xNear > xFar)
    {
        swap(&xNear, &xFar);
    }
    if(yNear > yFar)
    {
        swap(&yNear, &yFar);
    }
    
    if(xNear > yFar || yNear > xFar) 
    {
        return false;
    }
    
    f32 tNear = MAX(xNear, yNear);
    f32 tFar = MIN(xFar, yFar);
    
    if(tFar < 0.0f)
    {
        return false;
    }
    
    *t = tNear;
    
    contactPoint->x = origin.x + (dir.x * tNear);
    contactPoint->y = origin.y + (dir.y * tNear);
    
    if(xNear > yNear)
    {
        if(dir.x < 0)
        {
            contactNormal->x = -1.0f;
            contactNormal->y = 0.0f;
        }
        else
        {
            contactNormal->x = 1.0f;
            contactNormal->y = 0.0f;
            
        }
    }
    else if(xNear < yNear)
    {
        if(dir.y < 0)
        {
            contactNormal->x = 0.0f;
            contactNormal->y = -1.0f;
        }
        else
        {
            contactNormal->x = 0.0f;
            contactNormal->y = 1.0f;
        }
    }
    
    return true;
}

CollisionInfo DetectRectVsRectCollision(CollisionRect *rect, CollisionRect *target, Vector2 *velocity, f32 deltaTime)
{
    Vector2 origin = {rect->x, rect->y};
    Vector2 dir  = {velocity->x * deltaTime, velocity->y * deltaTime};
    f32 t = 0;
    Vector2 contactPoint = {0};
    Vector2 contactNormal = {0};
    
    CollisionInfo info = {0};
    info.collided = false;
    
    if(velocity->x == 0 && velocity->y == 0)
    {
        return info;
    }
    
    //NOTE(abhicv): rect after Minkowski Sum
    CollisionRect enlargedRect = {0};
    enlargedRect.x = target->x;
    enlargedRect.y = target->y;
    enlargedRect.width = target->width + rect->width;
    enlargedRect.height = target->height + rect->height;
    
    if(DetectRayRectCollision(origin, dir, &enlargedRect, &t, &contactPoint, &contactNormal))
    {
        if(t < 1.0f)
        {
            info.collided = true;
            info.normal = contactNormal;
            info.contactTime = t;
            
            if(contactNormal.x < 0.0)
            {
                info.penetrationDepth = (target->x + target->width / 2) - (rect->x - rect->width / 2) ;
            }
            else if(contactNormal.x > 0.0)
            {
                info.penetrationDepth = (rect->x + rect->width / 2) - (target->x - target->width / 2);
            }
            else if(contactNormal.y > 0.0)
            {
                info.penetrationDepth = (rect->y + rect->height / 2) - (target->y - target->height / 2);
            }
            else if(contactNormal.y < 0.0)
            {
                info.penetrationDepth = (target->y + target->height / 2) - (rect->y - rect->height / 2);
            }
            
            return info;
        }
    }
    
    return info;
}

//NOTE(abhicv): SAT collision test
CollisionInfo DetectCollision(CollisionRect *a, CollisionRect *b)
{
    CollisionInfo info = {0};
    
    Vector2 n = Vector2Subtract(Vector2Init(b->x, b->y), Vector2Init(a->x, a->y));
    
    f32 xOverlap = (a->width / 2) + (b->width / 2) - AbsoluteValueF32(n.x);
    
    if(xOverlap > 0.0f)
    {
        f32 yOverlap = (a->height / 2) + (b->height / 2) - AbsoluteValueF32(n.y);
        
        if(yOverlap > 0.0f)
        {
            if(xOverlap < yOverlap)
            {
                if(n.x < 0.0f)
                {
                    info.normal = Vector2Init(-1.0, 0.0);
                }
                else
                {
                    info.normal = Vector2Init(1.0, 0.0);
                }
                info.collided = true;
                info.penetrationDepth = xOverlap;
            }
            else
            {
                if(n.y < 0.0f)
                {
                    info.normal = Vector2Init(0.0, -1.0);
                }
                else
                {
                    info.normal = Vector2Init(0.0, 1.0);
                }
                info.collided = true;
                info.penetrationDepth = yOverlap;
            }
        }
    }
    
    return info;
}

void ResolveCollision(PhysicsBody *a, PhysicsBody *b, CollisionInfo info)
{
    Vector2 relativeVel = Vector2Subtract(b->velocity, a->velocity);
    f32 velAlongNormal = Vector2Dot(relativeVel, info.normal);
    f32 impulseMag = 0.0f;
    
    if(velAlongNormal > 0.0f)
    {
        return;
    }
    
    f32 minRestitution = a->restitution < b->restitution ? a->restitution : b->restitution;
    
    impulseMag = -(1.0f + minRestitution) * velAlongNormal / (a->inverseMass + b->inverseMass);
    
    Vector2 impulse = Vector2ScalarMultiply(info.normal , impulseMag);
    
    a->velocity.x -= impulse.x * a->inverseMass;
    a->velocity.y -= impulse.y * a->inverseMass;
    
    b->velocity.x += impulse.x * b->inverseMass;
    b->velocity.y += impulse.y * b->inverseMass;
    
    f32 percent = 0.2f;
    f32 slope = 0.05f;
    f32 p = (info.penetrationDepth - slope > 0.0f) ? (info.penetrationDepth - slope) : 0.0f;
    
    f32 inverseMassSum =  a->inverseMass + b->inverseMass;
    Vector2 correction = Vector2ScalarMultiply(info.normal, (p / inverseMassSum));
    
    a->position.x -= correction.x * a->inverseMass;
    a->position.y -= correction.y * a->inverseMass;
    
    b->position.x += correction.x * b->inverseMass;
    b->position.y += correction.y * b->inverseMass;
}
