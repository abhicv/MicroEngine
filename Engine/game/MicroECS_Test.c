#include "../src/MicroEngine.c"

#include "PlayerData.h"
#include "EnemyData.h"

#define MUI_ORIGIN_ID 1000

global MicroECSWorld ecsWorld;
global MUI ui;
global MUI_Input uiInput;
global TileMap tileMap;
global Entity player;
global Vector2 gravity = {0.0f, 500.0f};

f32 health = 0.5f;
char entityCountString[30] = "Entity Count";

char *EntityTagToString(u32 tag)
{
    char *tagName = 0;

    switch (tag)
    {
    case ENTITY_TAG_player:
        tagName = "ENTITY_TAG_player";
        break;
    case ENTITY_TAG_lizard:
        tagName = "ENTITY_TAG_lizard";
        break;
    case ENTITY_TAG_platform:
        tagName = "ENTITY_TAG_platform";
        break;
    default:
        tagName = "NoTag";
        break;
    }

    return tagName;
}

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);
    PlayerInputSystem(&ecsWorld.input, &event);
}

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);

    sprintf(entityCountString, "Active Entity Count : %d", ecsWorld.activeEntityCount);
    MUI_Rect textRect = {120, 40, 120, 30};
    MUI_Text(&ui, GEN_MUI_ID(), textRect, entityCountString, 16);

    PhysicsSystem(&ecsWorld, gravity, 0.016f);
    FiringSystem(&ecsWorld, &ecsWorld.input, &ecsWorld.stat[player], ecsWorld.transforms[player].position);

    int i = 0;
    loop(i, ecsWorld.activeEntityCount)
    {
        if (!IsEntityDead(i, &ecsWorld))
        {
            if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], AnimationSystemSignature))
            {
                AnimationSystem(&ecsWorld.animations[i]);
            }

            switch (ecsWorld.tags[i])
            {
            case ENTITY_TAG_player:
                if (ecsWorld.physics[player].collided)
                {
                    if (ecsWorld.physics[player].tagOfCollidedEntity == ENTITY_TAG_platform)
                    {
                        ecsWorld.physics[player].isGrounded = true;
                    }
                    else
                    {
                        ecsWorld.physics[player].isGrounded = true;
                    }
                }
                PlayerControlSystem(&ecsWorld.transforms[player], &ecsWorld.animations[player], 
                                        &ecsWorld.input, &ecsWorld.physics[player], &ecsWorld.stat[player]);
                break;

            case ENTITY_TAG_lizard:
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | ENTITYSTAT_COMPONENT_SIGN))
                {
                    EnemyPatrolSystem(&ecsWorld.transforms[i], &ecsWorld.stat[i], &ecsWorld.animations[i], deltaTime);
                    
                    //Enemy Health bar
                    Vector2 pos = ecsWorld.transforms[i].position;
                    MUI_Rect r = {pos.x, pos.y - 50, 60, 10};
                    ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, r);
                }

                if (ecsWorld.physics[i].collided)
                {
                    if (ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_bullet)
                    {
                        ecsWorld.stat[i].EnemyStat.health -= 0.1;
                    }
                }

                if (ecsWorld.stat[i].EnemyStat.health < 0.1)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                }

                break;
            
            case ENTITY_TAG_bullet:
                if (ecsWorld.physics[i].collided)
                {
                    if (ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_platform ||
                            ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_lizard )
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                    }
                }
                break;
            }   
        }
    }
}

void Render(SDL_Renderer *renderer)
{
    ME_RenderTileMap(&tileMap, renderer);

    int i = 0;
    loop(i, ecsWorld.activeEntityCount)
    {   
        if(!IsEntityDead(i, &ecsWorld))
        {
            if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], RenderSystemSignature))
            {
                RenderSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i], &ecsWorld.renders[i], renderer);
            }
            else if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | RENDER_COMPONENT_SIGN))
            {
                RenderSystemSimple(&ecsWorld.transforms[i], &ecsWorld.renders[i], renderer);
            }
            
            //Drawing collision rect for debug
            if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], PHYSICS_COMPONENT_SIGN))
            {
                SDL_Color c = {255, 255, 255, 0};
                SDL_Rect rect = {0};
                CollisionRect cRect = ecsWorld.physics[i].physicsBody.rect;
                rect.x = cRect.x - cRect.width / 2;
                rect.y = cRect.y - cRect.height / 2;
                rect.w = cRect.width;
                rect.h = cRect.height;

                ME_RenderDrawRect(renderer, &rect, c);
            }
        }
    }


    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{
    //Intialization
    ME_Game game = ME_CreateGame("MicroECS test", 1280, 720);
    SDL_Renderer *renderer = game.platform.renderer;

    tileMap = ME_LoadTileMap("default.tmap");
    tileMap.tileSheetTexture = IMG_LoadTexture(renderer, "assets/Sprites/tiles.png");

    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;
    
    ui.fontFile = OPENSANS_FONT_FILE;

    //player
    render.texture = IMG_LoadTexture(renderer, "assets/Sprites/Player.png");
    render.width = 64;
    render.height = 64;

    anim.animations[Idle] = idleAnim;
    anim.animations[Walking] = walkAnim;
    anim.animations[Jump] = jumpAnim;

    PhysicsComponent physics = {0};
    physics.physicsBody = CreatePhysicsBody(transform.position, 1.0, 30, 64);
    physics.physicsBody.velocity = Vector2Init(0.0f, 0.0f);
    physics.physicsBody.affectedByGravity = true;
    physics.physicsBody.restitution = 0.0f;
    physics.physicsBody.friction = 0.01f;
    physics.excludeEntityTag = ENTITY_TAG_none;

    player = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_player);
    if (player < MAX_ENTITY_COUNT)
    {
        ecsWorld.entitySignature[player] = TRANSFORM_COMPONENT_SIGN | 
                                            RENDER_COMPONENT_SIGN | 
                                            ANIMATION_COMPONENT_SIGN |
                                            ENTITYSTAT_COMPONENT_SIGN |
                                            PHYSICS_COMPONENT_SIGN;

        ecsWorld.transforms[player] = transform;
        ecsWorld.animations[player] = anim;
        ecsWorld.renders[player] = render;
        ecsWorld.physics[player] = physics;
        ecsWorld.stat[player] = PlayerStat;
    }
    else
    {
        printf("\nECS world full!!!\n");
        return 1;
    }

#if 0    
    //bullet
    u32 b = 0;
    loop(b, PlayerStat.PlayerStat.bulletCount)
    {
        Entity bullet = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_bullet);
        if (ENTITY_INDEX_VALID(bullet))
        {
            ecsWorld.entitySignature[bullet] = TRANSFORM_COMPONENT_SIGN | 
                                                    RENDER_COMPONENT_SIGN |
                                                    PHYSICS_COMPONENT_SIGN;

            render.width = 10;
            render.height = 10;
            render.texture = NULL;

            physics.physicsBody = CreatePhysicsBody(transform.position, 4.0f, render.width, render.height);
            physics.physicsBody.restitution = 0.0f; 
            physics.physicsBody.affectedByGravity = true;
            physics.excludeEntityTag = ENTITY_TAG_player | ENTITY_TAG_bullet | ENTITY_TAG_lizard;

            ecsWorld.transforms[bullet] = transform;
            ecsWorld.physics[bullet] = physics;
            ecsWorld.renders[bullet] = render; 
        }
        else
        {
            printf("\nECS world full!!!\n");
            return 1;
        }
    }    
#endif

    //platform
    FILE *colMapFile = fopen("default.cmap", "r");

    if(colMapFile != NULL)
    {
        u32 count = 0;
        fscanf(colMapFile, "n:%d\n", &count);

        u32 m = 0;
        loop(m, count)
        {
            Entity platform = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_platform);
            if (ENTITY_INDEX_VALID(platform))
            {
                ecsWorld.entitySignature[platform] = TRANSFORM_COMPONENT_SIGN | 
                                                        PHYSICS_COMPONENT_SIGN;

                i32 x,y;
                u32 w,h;

                fscanf(colMapFile, "c:%d,%d,%d,%d\n", &x, &y, &w, &h);

                transform.position = Vector2Init(x + w / 2, y + h / 2);
                render.width = w;
                render.height = h;
                render.texture = NULL;

                physics.physicsBody = CreatePhysicsBody(transform.position, 0.0f, render.width, render.height);
                physics.physicsBody.restitution = 0.0f; 
                physics.physicsBody.affectedByGravity = false;
                physics.excludeEntityTag = ENTITY_TAG_none;

                ecsWorld.transforms[platform] = transform;
                ecsWorld.physics[platform] = physics;
                ecsWorld.renders[platform] = render; 
            }
            else
            {
                printf("\nECS world full!!!\n");
                return 1;
            }
        }
    
    }
    
    //lizard
    lizardAnimation.animations[Idle] = lizardIdleAnim;
    lizardAnimation.animations[Walking] = lizardWalkAnim;

    lizardRender.texture = IMG_LoadTexture(renderer, "assets/Sprites/enemies.png");
    lizardRender.width = 64;
    lizardRender.height = 64;

    int n = 0;
    loop(n, 3)
    {
        u32 lizard = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_lizard);

        lizardTransform.position.x = ME_Random(100, 1000);
        lizardTransform.position.y = ME_Random(50, 600);

        lizardStat.EnemyStat.startPosition = lizardTransform.position;
        lizardStat.EnemyStat.moveRight = (n % 2); //odd lizard moves right
        lizardStat.EnemyStat.patrolDistance = 40;

        physics.physicsBody = CreatePhysicsBody(lizardTransform.position, 1.0, 50, 64);
        physics.physicsBody.restitution = 0.0f;
        physics.physicsBody.friction = 0.02f;
        physics.excludeEntityTag = ENTITY_TAG_none;

        if (lizard < MAX_ENTITY_COUNT)
        {
            ecsWorld.entitySignature[lizard] = TRANSFORM_COMPONENT_SIGN | 
                                                RENDER_COMPONENT_SIGN | 
                                                ANIMATION_COMPONENT_SIGN |
                                                ENTITYSTAT_COMPONENT_SIGN |
                                                PHYSICS_COMPONENT_SIGN;

            ecsWorld.transforms[lizard] = lizardTransform;
            ecsWorld.animations[lizard] = lizardAnimation;
            ecsWorld.renders[lizard] = lizardRender;
            ecsWorld.physics[lizard] = physics;
            ecsWorld.stat[lizard] = lizardStat;
        }
        else
        {
            printf("\nECS world full!!!\n");
            return 1;
        }
    }

    ME_RunGame(&game, false);

    ME_QuitGame(&game);

    return 0;
}