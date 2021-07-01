#include "system.h"
#include "..\Engine\src\ME_Audio.h"

void PlayerInputSystem(InputComponent *input, SDL_Event *event)
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
                input->jumpKeyDown = true;
            }
            
            input->jumpKeyHeld = true;
            break;
            
            case SDLK_LCTRL:
            if(!event->key.repeat)
            {
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
#define PLAYER_JUMP_SPEED 200.0f

void PlayerControlSystem(TransformComponent *transform, AnimationComponent *animation,
                         InputComponent *input, PhysicsComponent *physics,
                         EntityStatComponent *stat)
{
    if(physics->isGrounded)
    {
        animation->animations[Idle].flip = animation->animations[Walking].flip;
        animation->currentAnimationIndex = Idle;
    }
    else
    {
        animation->animations[Jump].flip = animation->animations[Walking].flip;
        animation->currentAnimationIndex = Jump;
    }
    
    if(input->leftKeyDown)
    {
        if(physics->isGrounded)
        {
            animation->currentAnimationIndex = Walking;
        }
        else
        {
            animation->currentAnimationIndex = Jump;
            animation->animations[animation->currentAnimationIndex].flip = true;
        }
        
        animation->animations[Walking].flip = true;
        
        stat->PlayerStat.facingDir = Left;
        physics->physicsBody.velocity.x = -PLAYER_MAX_SPEED_X;
    }
    
    if(input->rightKeyDown)
    {
        if(physics->isGrounded)
        {
            animation->currentAnimationIndex = Walking;
        }
        else
        {
            animation->currentAnimationIndex = Jump;
            animation->animations[animation->currentAnimationIndex].flip = false;
        }
        
        animation->animations[Walking].flip = false;
        
        stat->PlayerStat.facingDir = Right;
        physics->physicsBody.velocity.x = PLAYER_MAX_SPEED_X;
    }
    
    if(input->jumpKeyDown)
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

#define ENEMY_BULLET_SPEED 80.0f
#define ENEMT_CHASE_SPEED 100.0f
#define SQUARE(x) x*x

void FlyeeAISystem(MicroECSWorld *ecsWorld,
                   EntityStatComponent *stat,
                   TransformComponent *transform,
                   AnimationComponent *animation,
                   Vector2 playerPosition,
                   f32 deltaTime)
{
    Vector2 dirVec = Vector2Subtract(playerPosition, transform->position);
    f32 distanceSqr = Vector2SqrMag(dirVec);
    
    //simple Finite State Machine logic for enemy AI
    stat->EnemyStat.state = PATROL;
    
    if (distanceSqr < SQUARE(300.0f))
    {
        stat->EnemyStat.state = CHASE;
    }
    if (distanceSqr < SQUARE(200.0f))
    {
        stat->EnemyStat.shootDelay += deltaTime;
        stat->EnemyStat.state = SHOOT;
    }
    
    Vector2Normalize(&dirVec);
    
    if (stat->EnemyStat.state == CHASE)
    {
        transform->position.x = transform->position.x + (ENEMT_CHASE_SPEED * dirVec.x * deltaTime);
        transform->position.y = transform->position.y + (ENEMT_CHASE_SPEED * dirVec.y * deltaTime);
        
        animation->animations[Walking].flip = (dirVec.x < 0.0f) ? true : false;
    }
    else if (stat->EnemyStat.state == SHOOT && stat->EnemyStat.shootDelay > 0.5f)
    {
        stat->EnemyStat.shootDelay = 0.0f;
        
        //spawning a new bullet entity
        Entity bullet = MECS_CreateEntity(ecsWorld, ENTITY_TAG_ENEMY_BULLET);
        if (ENTITY_INDEX_VALID(bullet))
        {
            PhysicsComponent physics = {0};
            RenderComponent render = {0};
            
            Animation bulletAnim = {
                .frames = {
                    {32, 0},
                },
                .currentFrameIndex = 0,
                .frameInterval  = 100,
                .frameCount = 1,
                .flip = false,
            };
            
            AnimationComponent anim = {
                .animations = {0},
                .currentAnimationIndex = 0,
                .width = 16,
                .height = 16,
            };
            
            anim.animations[0] = bulletAnim;
            
            render.width = 30;
            render.height = 30;
            render.texture = NULL;
            
            physics.physicsBody = CreatePhysicsBody(playerPosition, 1.0f, 20, 20);
            physics.physicsBody.restitution = 0.0f;
            physics.physicsBody.affectedByGravity = false;
            physics.excludeEntityTag = ENTITY_TAG_BULLET | ENTITY_TAG_ENEMY_BULLET | ENTITY_TAG_FLYEE | ENTITY_TAG_COIN;
            
            ecsWorld->transforms[bullet].position = transform->position;
            ecsWorld->animations[bullet] = anim;
            ecsWorld->physics[bullet] = physics;
            ecsWorld->renders[bullet] = render;
            
            ecsWorld->entitySignature[bullet] = TRANSFORM_COMPONENT_SIGN |
                RENDER_COMPONENT_SIGN |
                ANIMATION_COMPONENT_SIGN |
                ENTITYSTAT_COMPONENT_SIGN |
                PHYSICS_COMPONENT_SIGN;
            
            ecsWorld->stat[bullet].BulletStat.startPosition = transform->position;
            
            ecsWorld->physics[bullet].physicsBody.velocity.x = ENEMY_BULLET_SPEED * dirVec.x;
            ecsWorld->physics[bullet].physicsBody.velocity.y = ENEMY_BULLET_SPEED * dirVec.y;
        }
    }
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
    }
}