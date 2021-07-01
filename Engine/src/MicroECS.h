#ifndef MICROECS_H
#define MICROECS_H

#include "types.h"
#include "ME_Vector2D.h"
#include "MicroPhysics.h"
#include "ME_debug.h"

#include <SDL2/SDL.h>

#define MAX_ENTITY_COUNT 100
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

//NOTE(abhicv): Used for indexing animations in animation component
enum AnimationState
{
    Idle,
    Walking,
    Jump,
    Attack,
    Dead,
    AnimationStateCount,
};

//NOTE(abhicv): Max number of frames an animation can have(subject to change depending on needs)
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
    //NOTE(abhicv): order of animations is important(should follow the order of AnimationState enum)
    Animation animations[AnimationStateCount];
    u32 currentAnimationIndex;
    
    //NOTE(abhicv): unit in pixels from sprite sheet
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

//NOTE(abhicv): all key map needed for the game control
typedef struct InputComponent
{
    bool upKeyDown;
    bool downKeyDown;
    bool leftKeyDown;
    bool rightKeyDown;
    bool jumpKeyDown;
    bool jumpKeyHeld;
    
    bool leftCtrlKeyDown;
    bool leftCtrlKeyHeld;
    
    i32 mouseX;
    i32 mouseY;
    
} InputComponent;

//NOTE(abhicv): states specifically for the game
typedef struct EntityStatComponent
{
    union
    {
        struct
        {
            f32 health;
            u32 facingDir;
            f32 affectHealthDelay;
            
        } PlayerStat;
        
        struct
        {
            f32 health;
            f32 patrolDistance;
            Vector2 startPosition;
            bool moveRight;
            u32 state;
            f32 shootDelay;
            
        } EnemyStat;
        
        struct 
        {
            Vector2 startPosition;
            
        } BulletStat;
    };
    
} EntityStatComponent;

//NOTE(abhicv): component signature
#define TRANSFORM_COMPONENT_SIGN   (1 << 0)
#define RENDER_COMPONENT_SIGN      (1 << 1)
#define ANIMATION_COMPONENT_SIGN   (1 << 2)
#define PHYSICS_COMPONENT_SIGN     (1 << 3)
#define ENTITYSTAT_COMPONENT_SIGN  (1 << 4)

//NOTE(abhicv): System Signatures
global const u32 AnimationSystemSignature = ANIMATION_COMPONENT_SIGN;
global const u32 RenderSystemSignature = TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | RENDER_COMPONENT_SIGN;
global const u32 PhysicsSystemSignature = TRANSFORM_COMPONENT_SIGN | PHYSICS_COMPONENT_SIGN;

//NOTE(abhicv): all components used in the game is owned by this struct
typedef struct MicroECSWorld
{
    TransformComponent transforms[MAX_ENTITY_COUNT];  
    RenderComponent renders[MAX_ENTITY_COUNT];        
    AnimationComponent animations[MAX_ENTITY_COUNT];  
    PhysicsComponent physics[MAX_ENTITY_COUNT];       
    EntityStatComponent stat[MAX_ENTITY_COUNT];       
    
    u32 entitySignature[MAX_ENTITY_COUNT];
    u32 tags[MAX_ENTITY_COUNT];
    bool entityDeathFlag[MAX_ENTITY_COUNT];
    
    u32 activeEntityCount;
    
    InputComponent input;
    
} MicroECSWorld;

enum EntityTag
{
    ENTITY_TAG_PLAYER =   (1 << 0),
    ENTITY_TAG_LIZARD =   (1 << 1),  
    ENTITY_TAG_SLIME =    (1 << 2),
    ENTITY_TAG_COIN =     (1 << 3),
    ENTITY_TAG_FLYEE =    (1 << 4),
    ENTITY_TAG_CAPTURE_FLAG = (1 << 5),
    
    ENTITY_TAG_NONE =     (1 << 6),   
    ENTITY_TAG_PLATFORM = (1 << 7),   
    ENTITY_TAG_BULLET =   (1 << 8),
    ENTITY_TAG_ENEMY_BULLET = (1 << 9),
};

#endif //MICROECS_H