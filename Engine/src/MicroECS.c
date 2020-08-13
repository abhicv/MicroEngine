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
    //NOTE(abhicv): finding an already dead entity index
    u32 i = 0;
    for(i = 0; i < world->activeEntityCount; i++)
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

u32 MECS_GetInActiveIndex(MicroECSWorld *world)
{
    u32 i = 0;
    for(i = 0; i < world->activeEntityCount; i++)
    {
        if(IsEntityDead(i, world))
        {
            world->entityDeathFlag[i] = false;
            return i;
        }
    }
    
    return 0;
}

TransformComponent CreateTransformComponent(Vector2 position, Vector2 size, f32 angle)
{
    TransformComponent transform = {0};
    
    transform.position = position;
    transform.size = size;
    transform.angle = angle;
    
    return transform;
}

void PlayerInputSystem(InputComponent *input, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:
            case SDLK_w:
            input->upKeyDown = true;
            break;
            
            case SDLK_DOWN:
            case SDLK_s:
            input->downKeyDown = true;
            break;
            
            case SDLK_LEFT:
            case SDLK_a:
            input->leftKeyDown = true;
            break;
            
            case SDLK_RIGHT:
            case SDLK_d:
            input->rightKeyDown = true;
            break;
            
            case SDLK_SPACE:
            input->jumpKeyDown = true;
            break;
            
            case SDLK_LCTRL:
            input->leftCtrlKeyDown = true;
            input->leftCtrlKeyHeld = true;
            break;
        }
        break;
        
        case SDL_KEYUP:
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:
            case SDLK_w:
            input->upKeyDown = false;
            break;
            
            case SDLK_DOWN:
            case SDLK_s:
            input->downKeyDown = false;
            break;
            
            case SDLK_LEFT:
            case SDLK_a:
            input->leftKeyDown = false;
            break;
            
            case SDLK_RIGHT:
            case SDLK_d:
            input->rightKeyDown = false;
            break;
            
            case SDLK_SPACE:
            input->jumpKeyDown = false;
            break;
            
            case SDLK_LCTRL:
            input->leftCtrlKeyDown = false;
            input->leftCtrlKeyHeld = false;
            break;
        }
    }
}

#define PLAYER_MAX_SPEED_X 100.0f
#define PLAYER_JUMP_SPEED 200.0f

void PlayerControlSystem(TransformComponent *transform,
                         AnimationComponent *animation,
                         InputComponent *input,
                         PhysicsComponent *physics,
                         EntityStatComponent *stat)
{
    
    animation->animations[Idle].flip = animation->animations[Walking].flip;
    animation->currentAnimationIndex = Idle;
    
    if(input->leftKeyDown)
    {
        animation->currentAnimationIndex = Walking;
        animation->animations[Walking].flip = true;
        stat->PlayerStat.facingDir = Left;
        physics->physicsBody.velocity.x = -PLAYER_MAX_SPEED_X;
    }
    
    if (input->rightKeyDown)
    {
        animation->currentAnimationIndex = Walking;
        animation->animations[Walking].flip = false;
        stat->PlayerStat.facingDir = Right;
        physics->physicsBody.velocity.x = PLAYER_MAX_SPEED_X;
    }
    
    if(input->jumpKeyDown)
    {
        animation->animations[Jump].flip = animation->animations[Walking].flip;
        animation->currentAnimationIndex = Jump;
        physics->physicsBody.velocity.y = -PLAYER_JUMP_SPEED;
        //input->jumpKeyDown = false;
    }
    
    if(input->leftCtrlKeyHeld)
    {
        animation->animations[Attack].flip = animation->animations[Walking].flip;
        animation->currentAnimationIndex = Attack;
    }
}

void EnemyPatrolSystem(TransformComponent *transform,
                       EntityStatComponent *stat,
                       AnimationComponent *animation,
                       f32 deltaTime)
{
    animation->currentAnimationIndex = Walking;
    
    local_persist f32 time = 0.0f;
    time += deltaTime;
    
    if(transform->position.x > (stat->EnemyStat.startPosition.x + stat->EnemyStat.patrolDistance))
    {
        stat->EnemyStat.moveRight = false;
    }
    if(transform->position.x < (stat->EnemyStat.startPosition.x - stat->EnemyStat.patrolDistance))
    {
        stat->EnemyStat.moveRight = true;
    }
    
    if(stat->EnemyStat.moveRight)
    {
        transform->position.x += 100.0f * deltaTime;
        animation->animations[Walking].flip = false;
    }
    else
    {
        transform->position.x += -100.0f * deltaTime;
        animation->animations[Walking].flip = true;
    }
    
    transform->position.y += (1.5f * sinf(SDL_GetTicks() / 100.0f));
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

#if 0
bool CheckCollison(CollisionRect *a, CollisionRect *b)
{
    i32 aLeft = a->x - a->width / 2;
    i32 aRight = a->x + a->width / 2;
    i32 aTop = a->y - a->height / 2;
    i32 aBottom = a->y + a->height / 2;
    
    i32 bLeft = b->x - b->width / 2;
    i32 bRight = b->x + b->width / 2;
    i32 bTop = b->y - b->height / 2;
    i32 bBottom = b->y + b->height / 2;
    
    if (aLeft > bRight || bLeft > aRight)
    {
        return false;
    }
    
    if (aTop > bBottom || bTop > aBottom)
    {
        return false;
    }
    
    return true;
}
#endif

void PhysicsSystem(MicroECSWorld *ecsWorld, Vector2 gravity ,f32 deltaTime)
{
    u32 i = 0;
    u32 j = 0;
    
    //NOTE(abhicv): Resetting all physics info
    for(i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature))
        {
            ecsWorld->physics[i].physicsBody.rect.x = ecsWorld->transforms[i].position.x;
            ecsWorld->physics[i].physicsBody.rect.y = ecsWorld->transforms[i].position.y;
            ecsWorld->physics[i].physicsBody.position = ecsWorld->transforms[i].position;
            ecsWorld->physics[i].collided = false;
            ecsWorld->physics[i].tagOfCollidedEntity = ENTITY_TAG_NONE;
            // ecsWorld->physics[i].isGrounded = false;
        }
    }
    
    for(i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        for(j = i + 1; j < ecsWorld->activeEntityCount; j++)
        {
            if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature) &&
               MECS_EntitySignatureEquals(ecsWorld->entitySignature[j], PhysicsSystemSignature) &&
               !IsEntityDead(i, ecsWorld) && !IsEntityDead(j, ecsWorld))
            {
                //NOTE(abhicv): avoiding entity in exclude entity tag
                if(!MECS_EntitySignatureEquals(ecsWorld->physics[i].excludeEntityTag, ecsWorld->tags[j]) &&
                   !MECS_EntitySignatureEquals(ecsWorld->physics[j].excludeEntityTag, ecsWorld->tags[i]))
                {
                    CollisionInfo info = DetectCollision(&ecsWorld->physics[i].physicsBody.rect, &ecsWorld->physics[j].physicsBody.rect);
                    
                    if(info.collided)
                    {
                        ecsWorld->physics[i].collided = true;
                        ecsWorld->physics[j].collided = true;
                        
                        ecsWorld->physics[i].collidedEntity = j;
                        ecsWorld->physics[j].collidedEntity = i;
                        
                        ecsWorld->physics[i].tagOfCollidedEntity = ecsWorld->tags[j];
                        ecsWorld->physics[j].tagOfCollidedEntity = ecsWorld->tags[i];
                        
                        ResolveCollision(&ecsWorld->physics[i].physicsBody, &ecsWorld->physics[j].physicsBody, info);
                    }
                }
            }
        }
    }
    
    //NOTE(abhicv): Applying all position changes based on physics calculations
    for(i = 0; i < ecsWorld->activeEntityCount; i++)
    {
        if(MECS_EntitySignatureEquals(ecsWorld->entitySignature[i], PhysicsSystemSignature))
        {
            UpdatePhysics(&ecsWorld->physics[i].physicsBody, gravity, deltaTime);
            ecsWorld->transforms[i].position = ecsWorld->physics[i].physicsBody.position;
        }
    }
}

void FiringSystem(MicroECSWorld *ecsWorld,
                  InputComponent *input,
                  EntityStatComponent *playerStat,
                  Vector2 playerPosition)
{
    TransformComponent transform = {0};
    PhysicsComponent physics = {0};
    RenderComponent render = {0};
    
    if(input->leftCtrlKeyDown)
    {
        Entity bullet = MECS_CreateEntity(ecsWorld, ENTITY_TAG_BULLET);
        
        if(ENTITY_INDEX_VALID(bullet))
        {
            ecsWorld->entitySignature[bullet] = TRANSFORM_COMPONENT_SIGN |
                RENDER_COMPONENT_SIGN |
                PHYSICS_COMPONENT_SIGN;
            
            render.width = 10;
            render.height = 10;
            
            physics.physicsBody = CreatePhysicsBody(playerPosition, 0.5f, render.width, render.height);
            physics.physicsBody.restitution = 0.0f;
            physics.physicsBody.affectedByGravity = false;
            physics.excludeEntityTag = ENTITY_TAG_PLAYER | ENTITY_TAG_BULLET;
            
            transform.position = playerPosition;
            
            ecsWorld->transforms[bullet] = transform;
            ecsWorld->physics[bullet] = physics;
            ecsWorld->renders[bullet] = render;
            
            if(playerStat->PlayerStat.facingDir == Left)
            {
                ecsWorld->physics[bullet].physicsBody.velocity.x = -600.0f;
            }
            
            if(playerStat->PlayerStat.facingDir == Right)
            {
                ecsWorld->physics[bullet].physicsBody.velocity.x = 600.0f;
            }
        }
        else
        {
            printf("\nECS world full!!!\n");
            return;
        }
        input->leftCtrlKeyDown = false;
    }
}

void RenderSystem(TransformComponent *transformComponent,
                  AnimationComponent *animationComponent,
                  RenderComponent *renderComponent,
                  SDL_Renderer *renderer)
{
    
    SDL_Rect srcRect = {0};
    SDL_Rect destRect = {0};
    
    u32 animationIndex = animationComponent->currentAnimationIndex;
    u32 frameIndex = animationComponent->animations[animationIndex].currentFrameIndex;
    
    srcRect.x = animationComponent->animations[animationIndex].frames[frameIndex].x;
    srcRect.y = animationComponent->animations[animationIndex].frames[frameIndex].y;
    srcRect.w = animationComponent->width;
    srcRect.h = animationComponent->height;
    
    destRect.x = (int)transformComponent->position.x - renderComponent->width / 2;
    destRect.y = (int)transformComponent->position.y - renderComponent->height / 2;
    destRect.w = renderComponent->width;
    destRect.h = renderComponent->height;
    
    
    u32 flipFlag = SDL_FLIP_NONE;
    if(animationComponent->animations[animationIndex].flip)
    {
        flipFlag = SDL_FLIP_HORIZONTAL;
    }
    
    if(renderer != NULL)
    {
        if(renderComponent->texture != NULL)
        {
            SDL_RenderCopyEx(renderer, renderComponent->texture, &srcRect, &destRect,
                             transformComponent->angle, NULL, flipFlag);
        }
    }
}

//NOTE(abhicv): simple rendering for entity without animation compontent
void RenderSystemSimple(TransformComponent *transformComponent,
                        RenderComponent *renderComponent,
                        SDL_Renderer *renderer)
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
            SDL_RenderCopyEx(renderer, renderComponent->texture, NULL, &destRect,
                             transformComponent->angle, NULL, SDL_FLIP_NONE);
        }
        else
        {
            SDL_Color c  = {255, 255, 0, 0};
            ME_RenderDrawRect(renderer, &destRect, c);
        }
    }
}