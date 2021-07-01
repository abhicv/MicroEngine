#include "..\Engine\src\MicroECS.h"
#include "data.h"

//NOTE(abhicv): returns player index so to address the player later from game logic code
Entity LoadPlayer(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity player = MECS_CreateEntity(ecsWorld, ENTITY_TAG_PLAYER);
    if (player < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[player] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        //render
        ecsWorld->renders[player].texture = gameResource->playerSprite;
        ecsWorld->renders[player].width = 64;
        ecsWorld->renders[player].height = 64;
        
        //animation
        Animation idleAnim = {
            .frames = {
                {16, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 60,
            .frameCount = 1,
            .flip = false,
        };
        
        Animation walkAnim = {
            .frames = {
                {0,  0},
                {16, 0},
                {32, 0},
                {16, 0},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 4,
            .flip = false,
        };
        
        Animation jumpAnim = {
            .frames = {
                {0, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 1,
            .flip = false,
        };
        
        Animation shootAnim = {
            .frames = {
                {48, 0},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 1,
            .flip = false,
        };
        
        ecsWorld->animations[player].animations[Idle] = idleAnim;
        ecsWorld->animations[player].animations[Walking] = walkAnim;
        ecsWorld->animations[player].animations[Jump] = jumpAnim;
        ecsWorld->animations[player].animations[Attack] = shootAnim;
        ecsWorld->animations[player].width = 16;
        ecsWorld->animations[player].height = 16;
        ecsWorld->animations[player].currentAnimationIndex = 0;
        
        //stat
        ecsWorld->stat[player].PlayerStat.health = 100;
        ecsWorld->stat[player].PlayerStat.facingDir = Right;
        
        //physics
        ecsWorld->physics[player].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 1.0, 45, 64);
        ecsWorld->physics[player].physicsBody.velocity = Vector2Init(0.0f, 0.0f);
        ecsWorld->physics[player].physicsBody.affectedByGravity = true;
        ecsWorld->physics[player].physicsBody.restitution = 0.0f;
        ecsWorld->physics[player].physicsBody.friction = 0.15f;
        ecsWorld->physics[player].excludeEntityTag = ENTITY_TAG_NONE;
        
        ecsWorld->entitySignature[player] = TRANSFORM_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            ENTITYSTAT_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
        
        return player;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
    
    return MAX_ENTITY_COUNT;
}

void LoadLizard(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity lizard = MECS_CreateEntity(ecsWorld, ENTITY_TAG_LIZARD);
    if (lizard < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[lizard]= CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        //animation
        Animation lizardIdleAnim = {
            .frames = {
                {0, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 60,
            .frameCount = 1,
            .flip = false,
        };
        
        Animation lizardWalkAnim = {
            .frames = {
                {0,  16},
                {16, 16},
                {32, 16},
                {48, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 4,
            .flip = false,
        };
        
        ecsWorld->animations[lizard].animations[Idle] = lizardIdleAnim;
        ecsWorld->animations[lizard].animations[Walking] = lizardWalkAnim;
        ecsWorld->animations[lizard].currentAnimationIndex = 1,
        ecsWorld->animations[lizard].width = 16,
        ecsWorld->animations[lizard].height = 16,
        
        //render
        ecsWorld->renders[lizard].texture = gameResource->enemySprite;
        ecsWorld->renders[lizard].width = 64;
        ecsWorld->renders[lizard].height = 64;
        
        //stats
        ecsWorld->stat[lizard].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->stat[lizard].EnemyStat.moveRight = !!(lizard % 2); //odd ones moves right
        ecsWorld->stat[lizard].EnemyStat.patrolDistance = 40.0f;
        ecsWorld->stat[lizard].EnemyStat.moveRight = false;
        ecsWorld->stat[lizard].EnemyStat.health = 1.0f;
        
        //physics
        ecsWorld->physics[lizard].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.5f, 64, 64);
        ecsWorld->physics[lizard].physicsBody.restitution = 0.0f;
        ecsWorld->physics[lizard].physicsBody.friction = 0.05f;
        ecsWorld->physics[lizard].excludeEntityTag = ENTITY_TAG_NONE;
        
        ecsWorld->entitySignature[lizard] = TRANSFORM_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            ENTITYSTAT_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}

void LoadSlime(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity slime = MECS_CreateEntity(ecsWorld, ENTITY_TAG_SLIME);
    if (slime < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[slime] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        //render
        ecsWorld->renders[slime].texture = gameResource->enemySprite;
        ecsWorld->renders[slime].width = 50;
        ecsWorld->renders[slime].height = 50;
        
        //animation
        Animation slimeIdleAnim = {
            .frames = {
                {0, 0},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 60,
            .frameCount = 1,
            .flip = false,
        };
        
        Animation slimeWalkAnim = {
            .frames = {
                {0,  0},
                {16, 0},
                {32, 0},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 3,
            .flip = false,
        };
        ecsWorld->animations[slime].animations[Idle] = slimeIdleAnim;
        ecsWorld->animations[slime].animations[Walking] = slimeWalkAnim;
        ecsWorld->animations[slime].width = 16;
        ecsWorld->animations[slime].height = 16;
        ecsWorld->animations[slime].currentAnimationIndex = 1;
        
        //stat
        ecsWorld->stat[slime].EnemyStat.health = 1.0f;
        ecsWorld->stat[slime].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->stat[slime].EnemyStat.moveRight = !!(slime % 2); //odd ones moves right
        ecsWorld->stat[slime].EnemyStat.patrolDistance = 50.0f;
        
        //physics
        PhysicsComponent physics = {0};
        ecsWorld->physics[slime].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.5, 45, 50);
        ecsWorld->physics[slime].physicsBody.restitution = 0.0f;
        ecsWorld->physics[slime].physicsBody.friction = 0.05f;
        ecsWorld->physics[slime].excludeEntityTag = ENTITY_TAG_NONE;
        
        ecsWorld->entitySignature[slime] = TRANSFORM_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            ENTITYSTAT_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}

void LoadFlyee(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity flyee = MECS_CreateEntity(ecsWorld, ENTITY_TAG_FLYEE);
    if (flyee < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[flyee] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        //render
        ecsWorld->renders[flyee].texture = gameResource->enemySprite;
        ecsWorld->renders[flyee].width = 50;
        ecsWorld->renders[flyee].height = 50;
        
        //animation
        Animation IdleAnim = {
            .frames = {
                {0, 32},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 60,
            .frameCount = 1,
            .flip = false,
        };
        
        Animation WalkAnim = {
            .frames = {
                {0,  32},
                {16, 32},
                {32, 32},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 150,
            .frameCount = 3,
            .flip = false,
        };
        ecsWorld->animations[flyee].animations[Idle] = IdleAnim;
        ecsWorld->animations[flyee].animations[Walking] = WalkAnim;
        ecsWorld->animations[flyee].width = 16;
        ecsWorld->animations[flyee].height = 16;
        ecsWorld->animations[flyee].currentAnimationIndex = 1;
        
        //stat
        ecsWorld->stat[flyee].EnemyStat.health = 1.0f;
        ecsWorld->stat[flyee].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->stat[flyee].EnemyStat.moveRight = !!(flyee % 2); //odd ones moves right
        ecsWorld->stat[flyee].EnemyStat.patrolDistance = 50.0f;
        ecsWorld->stat[flyee].EnemyStat.state = PATROL;
        
        //physics
        PhysicsComponent physics = {0};
        ecsWorld->physics[flyee].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 200.0f, 45, 30);
        ecsWorld->physics[flyee].physicsBody.restitution = 0.0f;
        ecsWorld->physics[flyee].physicsBody.friction = 0.05f;
        ecsWorld->physics[flyee].excludeEntityTag = ENTITY_TAG_PLATFORM | ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_COIN;
        ecsWorld->physics[flyee].physicsBody.affectedByGravity = false;
        
        ecsWorld->entitySignature[flyee] = TRANSFORM_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            ENTITYSTAT_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}

void LoadCoin(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity coin  = MECS_CreateEntity(ecsWorld, ENTITY_TAG_COIN);
    if (coin < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[coin]= CreateTransformComponent(Vector2Init((f32)x, (f32)y),
                                                             Vector2Null(), 0.0f);
        
        //animation
        Animation coinAnim = {
            .frames = {
                {0, 16},
                {16, 16},
                {32, 16},
                {48, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 100,
            .frameCount = 4,
            .flip = false,
        };
        ecsWorld->animations[coin].animations[Idle] = coinAnim;
        ecsWorld->animations[coin].width = 16;
        ecsWorld->animations[coin].height = 16;
        ecsWorld->animations[coin].currentAnimationIndex = Idle;
        
        //render
        ecsWorld->renders[coin].texture = gameResource->itemSprite;
        ecsWorld->renders[coin].width = 40;
        ecsWorld->renders[coin].height = 40;
        
        //physics
        ecsWorld->physics[coin].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 1.0, 32, 32);
        ecsWorld->physics[coin].physicsBody.restitution = 0.0f;
        ecsWorld->physics[coin].physicsBody.friction = 0.07f;
        ecsWorld->physics[coin].excludeEntityTag = ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_BULLET;
        ecsWorld->physics[coin].physicsBody.affectedByGravity = false;
        
        ecsWorld->entitySignature[coin] = TRANSFORM_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}
