#include "system.h" 
#include "..\Engine\src\ME_Audio.h"

void PlayerInputSystem(InputComponent *input, SDL_Event *event, GameResource *gameResource)
{
    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:
            input->upKeyDown = true;
            break;
            
            case SDLK_DOWN:
            input->downKeyDown = true;
            break;
            
            case SDLK_LEFT:
            input->leftKeyDown = true;
            break;
            
            case SDLK_RIGHT:
            input->rightKeyDown = true;
            break;
            
            case SDLK_SPACE:
            if(!event->key.repeat)
            {
                ME_PlayAudio(&gameResource->jumpSound);
                input->jumpKeyDown = true;
            }
            input->jumpKeyHeld = true;
            break;
            
            case SDLK_LCTRL:
            if(!event->key.repeat)
            {
                ME_PlayAudio(&gameResource->shootSound);
                input->leftCtrlKeyDown = true;
            }
            input->leftCtrlKeyHeld = true;
            break;
        }
        break;
        
        case SDL_KEYUP:
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:
            input->upKeyDown = false;
            break;
            
            case SDLK_DOWN:
            input->downKeyDown = false;
            break;
            
            case SDLK_LEFT:
            input->leftKeyDown = false;
            break;
            
            case SDLK_RIGHT:
            input->rightKeyDown = false;
            break;
            
            case SDLK_SPACE:
            input->jumpKeyDown = false;
            input->jumpKeyHeld = false;
            break;
            
            case SDLK_LCTRL:
            input->leftCtrlKeyDown = false;
            input->leftCtrlKeyHeld = false;
            break;
        }
        break;
    }
}

#define PLAYER_MAX_SPEED_X 100.0f
#define PLAYER_JUMP_SPEED 250.0f

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
    
    if(input->rightKeyDown)
    {
        animation->currentAnimationIndex = Walking;
        animation->animations[Walking].flip = false;
        stat->PlayerStat.facingDir = Right;
        physics->physicsBody.velocity.x = PLAYER_MAX_SPEED_X;
    }
    
    if(input->jumpKeyHeld)
    {
        animation->animations[Jump].flip = animation->animations[Walking].flip;
        animation->currentAnimationIndex = Jump;
    }
    
    if(input->jumpKeyDown && physics->isGrounded)
    {
        physics->physicsBody.velocity.y = -PLAYER_JUMP_SPEED;
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
    
    //transform->position.y += (sinf(SDL_GetTicks() / 100.0f));
}

#define BULLET_SPEED 150
void FiringSystem(MicroECSWorld *ecsWorld,
                  InputComponent *input,
                  EntityStatComponent *playerStat,
                  Vector2 playerPosition)
{
    TransformComponent transform = {0};
    PhysicsComponent physics = {0};
    RenderComponent render = {0};
    
    Animation bulletAnim = {
        .frames = {
            {0, 32},
            {16, 32},
            {32, 32},
        },
        .currentFrameIndex = 0,
        .frameInterval  = 100,
        .frameCount = 3,
        .flip = false,
    };
    
    bulletAnim.flip = (playerStat->PlayerStat.facingDir == Right) ? false : true;
    
    AnimationComponent anim = {
        .animations = {0},
        .currentAnimationIndex = 0,
        .width = 16,
        .height = 16,
    };
    
    anim.animations[0] = bulletAnim;
    
    if(input->leftCtrlKeyDown)
    {
        Entity bullet = MECS_CreateEntity(ecsWorld, ENTITY_TAG_BULLET);
        if(ENTITY_INDEX_VALID(bullet))
        {
            ecsWorld->entitySignature[bullet] = TRANSFORM_COMPONENT_SIGN |
                RENDER_COMPONENT_SIGN |
                ANIMATION_COMPONENT_SIGN |
                ENTITYSTAT_COMPONENT_SIGN |
                PHYSICS_COMPONENT_SIGN;
            
            transform.position = playerPosition;
            
            render.width = 45;
            render.height = 45;
            render.texture = NULL;
            
            physics.physicsBody = CreatePhysicsBody(playerPosition, 2.0f, 30, 15);
            physics.physicsBody.restitution = 0.0f;
            physics.physicsBody.affectedByGravity = false;
            physics.excludeEntityTag = ENTITY_TAG_PLAYER | ENTITY_TAG_BULLET;
            
            ecsWorld->transforms[bullet] = transform;
            ecsWorld->animations[bullet] = anim;
            ecsWorld->physics[bullet] = physics;
            ecsWorld->renders[bullet] = render;
            
            ecsWorld->stat[bullet].BulletStat.startPosition = transform.position;
            
            if(playerStat->PlayerStat.facingDir == Left)
            {
                ecsWorld->physics[bullet].physicsBody.velocity.x = -BULLET_SPEED;
            }
            
            if(playerStat->PlayerStat.facingDir == Right)
            {
                ecsWorld->physics[bullet].physicsBody.velocity.x = BULLET_SPEED;
            }
        }
        else
        {
            printf("\nECS world full!!!\n");
            return;
        }
        
        //input->leftCtrlKeyDown = false;
    }
}