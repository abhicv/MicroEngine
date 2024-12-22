#include "MicroECS.h"
#include "ME_Utility.h"

bool MECS_EntitySignatureEquals(u32 entitySignature, u32 signature)
{
    u32 masked = (entitySignature & signature);
    return (masked == signature);
}

bool IsEntityDead(u32 index, MicroECSWorld *world)
{
    return world->entityDeathFlag[index];
}

u32 MECS_CreateEntity(MicroECSWorld *world, u32 tag)
{
    // reusung dead entity index
    for(u32 i = 0; i < world->activeEntityCount; i++)
    {
        if(IsEntityDead(i, world))
        {
            world->tags[i] = tag;
            world->entityDeathFlag[i] = false;
            return i;
        }
    }
    
    if(world->activeEntityCount < MAX_ENTITY_COUNT)
    {
        u32 entityIndex = world->activeEntityCount;
        world->tags[entityIndex] = tag;
        world->activeEntityCount++;
        
        return entityIndex;
    }
    
    return INVALID_ENTITY_INDEX;
}

TransformComponent CreateTransformComponent(Vector2 position, Vector2 size, f32 angle)
{
    TransformComponent transform = {0};
    
    transform.position = position;
    transform.size = size;
    transform.angle = angle;
    
    return transform;
}

void AnimationSystem(AnimationComponent *animationComponent)
{
    u32 index = animationComponent->currentAnimationIndex;
    Animation *currentAnimation = &animationComponent->animations[index];
    
    if(currentAnimation != NULL)
    {
        currentAnimation->currentFrameIndex = (SDL_GetTicks() / currentAnimation->frameInterval) % currentAnimation->frameCount;
    }
    else
    {
        printf("Invalid animation index\n");
    }
}

void PhysicsSystem(MicroECSWorld *ecsWorld, Vector2 gravity ,f32 deltaTime)
{
    // Resetting all physics info
    for(u32 i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature))
        {
            ecsWorld->physics[i].physicsBody.rect.x = ecsWorld->transforms[i].position.x;
            ecsWorld->physics[i].physicsBody.rect.y = ecsWorld->transforms[i].position.y;
            ecsWorld->physics[i].physicsBody.position = ecsWorld->transforms[i].position;
            ecsWorld->physics[i].physicsBody.collisionNormal = Vector2Null();
            ecsWorld->physics[i].collided = false;
            ecsWorld->physics[i].collisionCount = 0;
            ecsWorld->physics[i].tagOfCollidedEntity = 0;
            ecsWorld->physics[i].isGrounded = false;
        }
    }
    
    for(u32 i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        for(u32 j = i + 1; j < ecsWorld->activeEntityCount; j++)
        {
            if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature) &&
               MECS_EntitySignatureEquals(ecsWorld->entitySignature[j], PhysicsSystemSignature) &&
               !IsEntityDead(i, ecsWorld) && !IsEntityDead(j, ecsWorld))
            {
                CollisionInfo info = DetectCollision(&ecsWorld->physics[i].physicsBody.rect, &ecsWorld->physics[j].physicsBody.rect);
                                
                if(info.collided)
                {
                    ecsWorld->physics[i].collided = true;
                    ecsWorld->physics[j].collided = true;

                    if(ecsWorld->physics[i].collisionCount < MAX_COLLISION_COUNT)
                    {
                        ecsWorld->physics[i].collidedEntities[ecsWorld->physics[i].collisionCount] = j;
                        ecsWorld->physics[i].normals[ecsWorld->physics[i].collisionCount] = info.normal;
                        ecsWorld->physics[i].tagOfCollidedEntity |= ecsWorld->tags[j];
                        ecsWorld->physics[i].collisionCount++;
                    }

                    if(ecsWorld->physics[j].collisionCount < MAX_COLLISION_COUNT)
                    {
                        ecsWorld->physics[j].collidedEntities[ecsWorld->physics[j].collisionCount] = i;
                        ecsWorld->physics[j].normals[ecsWorld->physics[j].collisionCount] = (Vector2){-info.normal.x, -info.normal.y};
                        ecsWorld->physics[j].tagOfCollidedEntity |= ecsWorld->tags[i];
                        ecsWorld->physics[j].collisionCount++;
                    }

                    // ecsWorld->physics[i].physicsBody.collisionNormal = info.normal;
                    // ecsWorld->physics[j].physicsBody.collisionNormal = (Vector2){-info.normal.x, -info.normal.y};

                    if(!MECS_EntitySignatureEquals(ecsWorld->physics[i].excludeEntityTag, ecsWorld->tags[j]) &&
                        !MECS_EntitySignatureEquals(ecsWorld->physics[j].excludeEntityTag, ecsWorld->tags[i]))
                    {
                        ResolveCollision(&ecsWorld->physics[i].physicsBody, &ecsWorld->physics[j].physicsBody, info);
                    }
                }
            }
        }        
    }
    
    // Applying all position changes based on physics calculations
    for(u32 i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature))
        {
            UpdatePhysics(&ecsWorld->physics[i].physicsBody, gravity, deltaTime);
            ecsWorld->transforms[i].position = ecsWorld->physics[i].physicsBody.position;
        }
    }
}

void RenderSystem(TransformComponent *transformComponent, AnimationComponent *animationComponent,
                  RenderComponent *renderComponent, SDL_Renderer *renderer)
{
    
    u32 animationIndex = animationComponent->currentAnimationIndex;
    u32 frameIndex = animationComponent->animations[animationIndex].currentFrameIndex;
    
    SDL_Rect srcRect = {0};
    srcRect.x = animationComponent->animations[animationIndex].frames[frameIndex].x;
    srcRect.y = animationComponent->animations[animationIndex].frames[frameIndex].y;
    srcRect.w = animationComponent->width;
    srcRect.h = animationComponent->height;
    
    SDL_Rect destRect = {0};
    destRect.x = (int)transformComponent->position.x - renderComponent->width / 2;
    destRect.y = (int)transformComponent->position.y - renderComponent->height / 2;
    destRect.w = renderComponent->width;
    destRect.h = renderComponent->height;
    
    u32 flipFlag = animationComponent->animations[animationIndex].flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    
    if(renderer != NULL && renderComponent->texture != NULL)
    {
        SDL_RenderCopyEx(renderer, renderComponent->texture, &srcRect, &destRect, transformComponent->angle, NULL, flipFlag);
    }
}

// Simple rendering for entity without animation component
void RenderSystemSimple(TransformComponent *transformComponent, RenderComponent *renderComponent, SDL_Renderer *renderer)
{
    SDL_Rect destRect = {0};
    destRect.x = (int)transformComponent->position.x - renderComponent->width / 2;
    destRect.y = (int)transformComponent->position.y - renderComponent->height / 2;
    destRect.w = renderComponent->width;
    destRect.h = renderComponent->height;
    
    if(renderer != NULL)
    {
        if(renderComponent->texture != NULL)
        {
            SDL_RenderCopyEx(renderer, renderComponent->texture, NULL, &destRect, transformComponent->angle, NULL, SDL_FLIP_NONE);
        }
        else
        {
            SDL_Color c  = {255, 255, 255, 0};
            ME_RenderFillRect(renderer, &destRect, c);
        }
    }
}
