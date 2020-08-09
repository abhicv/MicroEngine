#ifndef MICROECS_H
#define MICROECS_H

#include "types.h"
#include "ME_Vector2D.h"
#include "MicroPhysics.h"
#include "ME_debug.h"

#include <SDL2/SDL.h>

#define MAX_ENTITY_COUNT 50
#define INVALID_ENTITY_INDEX MAX_ENTITY_COUNT
#define ENTITY_INDEX_VALID(index) index < MAX_ENTITY_COUNT

typedef u32 Entity;

typedef struct TransformComponent
{
    Vector2 position;
    Vector2 size;
    f32 angle;
    
} TransformComponent;

typedef struct RenderComponent
{
    u32 width;
    u32 height;
    SDL_Texture *texture;
    
} RenderComponent;

enum Direction
{
    Right,
    Left,
};

//NOTE(lonecoder): Used for indexing animations in animation component
enum AnimationState
{
    Idle,
    Walking,
    Jump,
    Attack,
    Dead,
    AnimationStateCount,
};

//NOTE(lonecoder): Max number of frames an animation can have(subject to change depending on needs)
#define MAX_FRAME_COUNT 10

typedef struct AnimationFrame
{
    i32 x;
    i32 y;
    
} AnimationFrame;

typedef struct Animation
{
    AnimationFrame frames[MAX_FRAME_COUNT];
    u32 currentFrameIndex;
    u32 frameInterval;
    u32 frameCount;
    bool flip;
    
} Animation;

typedef struct AnimationComponent
{
    //NOTE(lonecoder): order of animations is important(should follow the order of AnimationState enum)
    Animation animations[AnimationStateCount];
    u32 currentAnimationIndex;
    
    //NOTE(lonecoder): unit in pixels from sprite sheet
    u32 width;
    u32 height;
    
} AnimationComponent;

typedef struct PhysicsComponent
{
    PhysicsBody physicsBody;
    Entity collidedEntity;
    u32 tagOfCollidedEntity;
    bool collided;
    bool isGrounded;
    u32 excludeEntityTag;
    
} PhysicsComponent;

//NOTE(lonecoder): all key map needed for the game control
typedef struct InputComponent
{
    bool upKeyDown;
    bool downKeyDown;
    bool leftKeyDown;
    bool rightKeyDown;
    bool jumpKeyDown;
    
    bool leftCtrlKeyDown;
    bool leftCtrlKeyHeld;
    
    i32 mouseX;
    i32 mouseY;
    
} InputComponent;

//NOTE(lonecoder): stats specifically for the game
typedef struct EntityStatComponent
{
    union
    {
        struct
        {
            f32 health;
            u32 facingDir;
            
        } PlayerStat;
        
        struct
        {
            f32 health;
            f32 patrolDistance;
            Vector2 startPosition;
            bool moveRight;
            
        } EnemyStat;
    };
    
} EntityStatComponent;

//NOTE(lonecoder): component signature
#define TRANSFORM_COMPONENT_SIGN  (1 << 0)
#define RENDER_COMPONENT_SIGN (1 << 1)
#define ANIMATION_COMPONENT_SIGN (1 << 2)
#define PHYSICS_COMPONENT_SIGN (1 << 3)
#define ENTITYSTAT_COMPONENT_SIGN (1 << 4)

//NOTE(lonecoder): System Signatures
global u32 AnimationSystemSignature = ANIMATION_COMPONENT_SIGN;
global u32 RenderSystemSignature = TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | RENDER_COMPONENT_SIGN;
global u32 PhysicsSystemSignature = TRANSFORM_COMPONENT_SIGN | PHYSICS_COMPONENT_SIGN;

//NOTE(lonecoder): all components used in the game is owned by this struct
typedef struct MicroECSWorld
{
    TransformComponent transforms[MAX_ENTITY_COUNT];  //0
    RenderComponent renders[MAX_ENTITY_COUNT];        //1
    AnimationComponent animations[MAX_ENTITY_COUNT];  //2
    PhysicsComponent physics[MAX_ENTITY_COUNT];       //3
    EntityStatComponent stat[MAX_ENTITY_COUNT];       //4
    
    u32 entitySignature[MAX_ENTITY_COUNT];
    u32 tags[MAX_ENTITY_COUNT];
    bool entityDeathFlag[MAX_ENTITY_COUNT];
    
    u32 activeEntityCount; //number of currently active entities created
    
    InputComponent input; //input only for main player
    
} MicroECSWorld;

enum Entitytag
{
    ENTITY_TAG_PLAYER =   (1 << 0),   //1
    ENTITY_TAG_BULLET =   (1 << 1),   //2
    ENTITY_TAG_LIZARD =   (1 << 2),   //4
    ENTITY_TAG_PLATFORM = (1 << 3), //8
    ENTITY_TAG_NONE =     (1 << 4),     //16
};

#endif