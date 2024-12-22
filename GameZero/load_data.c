#include "..\Engine\src\MicroECS.h"
#include "data.h"

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
        
        //state
        ecsWorld->states[player].Player.health = 100;
        ecsWorld->states[player].Player.facingDir = Right;
        
        //physics
        ecsWorld->physics[player].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 1.0, 45, 64);
        ecsWorld->physics[player].physicsBody.velocity = Vector2Init(0.0f, 0.0f);
        ecsWorld->physics[player].physicsBody.affectedByGravity = true;
        ecsWorld->physics[player].physicsBody.restitution = 0.0f;
        ecsWorld->physics[player].physicsBody.friction = 0.25f;
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
        ecsWorld->states[lizard].Enemy.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->states[lizard].Enemy.moveRight = !!(lizard % 2); //odd ones moves right
        ecsWorld->states[lizard].Enemy.patrolDistance = 40.0f;
        ecsWorld->states[lizard].Enemy.moveRight = false;
        ecsWorld->states[lizard].Enemy.health = 1.0f;
        
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
        ecsWorld->states[slime].Enemy.health = 1.0f;
        ecsWorld->states[slime].Enemy.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->states[slime].Enemy.moveRight = !!(slime % 2); //odd ones moves right
        ecsWorld->states[slime].Enemy.patrolDistance = 50.0f;
        
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
        ecsWorld->states[flyee].Enemy.health = 1.0f;
        ecsWorld->states[flyee].Enemy.startPosition = Vector2Init((f32)x, (f32)y);
        ecsWorld->states[flyee].Enemy.moveRight = !!(flyee % 2); //odd ones moves right
        ecsWorld->states[flyee].Enemy.patrolDistance = 50.0f;
        ecsWorld->states[flyee].Enemy.state = PATROL;
        
        //physics
        PhysicsComponent physics = {0};
        ecsWorld->physics[flyee].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.0f, 45, 30);
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

void LoadCrate(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity crate  = MECS_CreateEntity(ecsWorld, ENTITY_TAG_CRATE);
    
    if (crate < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[crate]= CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        Animation anim = {
            .frames = {
                {64, 0},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 100,
            .frameCount = 1,
            .flip = false,
        };
        
        ecsWorld->animations[crate].animations[Idle] = anim;
        ecsWorld->animations[crate].width = 16;
        ecsWorld->animations[crate].height = 16;
        ecsWorld->animations[crate].currentAnimationIndex = Idle;
        
        //render
        ecsWorld->renders[crate].texture = gameResource->itemSprite;
        ecsWorld->renders[crate].width = 50;
        ecsWorld->renders[crate].height = 50;
        
        //physics
        ecsWorld->physics[crate].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 10.0f, 50, 50);
        ecsWorld->physics[crate].physicsBody.restitution = 0.0f;
        ecsWorld->physics[crate].physicsBody.friction = 0.1f;
        ecsWorld->physics[crate].excludeEntityTag = ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_BULLET;
        ecsWorld->physics[crate].physicsBody.affectedByGravity = true;
        
        ecsWorld->entitySignature[crate] = TRANSFORM_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}

void LoadCaptureFlag(MicroECSWorld *ecsWorld, GameResource *gameResource, int x, int y)
{
    Entity flag  = MECS_CreateEntity(ecsWorld, ENTITY_TAG_CAPTURE_FLAG);
    
    if (flag < MAX_ENTITY_COUNT)
    {
        //transform
        ecsWorld->transforms[flag]= CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
        
        Animation anim = {
            .frames = {
                {64, 16},
            },
            .currentFrameIndex = 0,
            .frameInterval  = 100,
            .frameCount = 1,
            .flip = false,
        };
        
        ecsWorld->animations[flag].animations[Idle] = anim;
        ecsWorld->animations[flag].width = 16;
        ecsWorld->animations[flag].height = 16;
        ecsWorld->animations[flag].currentAnimationIndex = Idle;
        
        //render
        ecsWorld->renders[flag].texture = gameResource->itemSprite;
        ecsWorld->renders[flag].width = 50;
        ecsWorld->renders[flag].height = 50;
        
        //physics
        ecsWorld->physics[flag].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 10.0f, 50, 50);
        ecsWorld->physics[flag].physicsBody.restitution = 0.0f;
        ecsWorld->physics[flag].physicsBody.friction = 0.1f;
        ecsWorld->physics[flag].excludeEntityTag = 0xFFFFFFFF & (~ENTITY_TAG_PLATFORM);  //exclude all entity except platform

        ecsWorld->physics[flag].physicsBody.affectedByGravity = true;
        
        ecsWorld->entitySignature[flag] = TRANSFORM_COMPONENT_SIGN |
            ANIMATION_COMPONENT_SIGN |
            RENDER_COMPONENT_SIGN |
            PHYSICS_COMPONENT_SIGN;
    }
    else
    {
        printf("\nECS world full!!!\n");
    }
}
