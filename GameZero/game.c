#include <SDL2/SDL.h>

#include "../Engine/src/MicroEngine.c"
#include "PlayerData.h"
#include "EnemyData.h"

#define MUI_ORIGIN_ID 1020

global MicroECSWorld ecsWorld;
global MUI ui;
global MUI_Input uiInput;
global TileMap tileMap;
global Entity player;
global Vector2 gravity = {0.0f, 100.0f};

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
}

void HandleEvent(SDL_Event *event)
{
    MUI_GetInput(&uiInput, event);
    PlayerInputSystem(&ecsWorld.input, event);
}

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);

    sprintf(entityCountString, "Active Entity Count : %d", ecsWorld.activeEntityCount);
    MUI_Rect textRect = {120, 20, 120, 30};
    MUI_Text(&ui, GEN_MUI_ID(), textRect, entityCountString, 15);

    PhysicsSystem(&ecsWorld, gravity, 0.016f);
    FiringSystem(&ecsWorld, &ecsWorld.input, &ecsWorld.stat[player], ecsWorld.transforms[player].position);

    u32 i = 0;
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
                    MUI_Rect r = {pos.x, pos.y - 50, 60, 5};
                    ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, r);
                }

                if(ecsWorld.physics[i].collided)
                {
                    if(ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_bullet)
                    {
                        ecsWorld.stat[i].EnemyStat.health -= 0.1;
                    }
                }

                if(ecsWorld.stat[i].EnemyStat.health < 0.1)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                }

                break;
            
            case ENTITY_TAG_bullet:
                if(ecsWorld.physics[i].collided)
                {
                    if(ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_platform ||
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

    u32 i = 0;
    loop(i, ecsWorld.activeEntityCount)
    {   
        if(!IsEntityDead(i, &ecsWorld))
        {
            if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], RenderSystemSignature))
            {
                RenderSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i], &ecsWorld.renders[i], renderer);
            }
            else if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | RENDER_COMPONENT_SIGN))
            {
                RenderSystemSimple(&ecsWorld.transforms[i], &ecsWorld.renders[i], renderer);
            }
            
            //Drawing collision rect for debug
            if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], PHYSICS_COMPONENT_SIGN))
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

void LoadAssets(SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;

    tileMap = ME_LoadTileMap("default.tmap");
    tileMap.tileSheetTexture = IMG_LoadTexture(renderer, "assets/Sprites/tiles.png");
    
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
    physics.physicsBody.friction = 0.05f;
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
    }

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
            }
        }

        fclose(colMapFile);
    }
    
    //lizard
    lizardAnimation.animations[Idle] = lizardIdleAnim;
    lizardAnimation.animations[Walking] = lizardWalkAnim;

    lizardRender.texture = IMG_LoadTexture(renderer, "assets/Sprites/enemies.png");
    lizardRender.width = 64;
    lizardRender.height = 64;

    int n = 0;
    loop(n, 6)
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
    }
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event = {0};
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize TTF : %s\n", SDL_GetError());
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());
    }

    window = SDL_CreateWindow("GameZero", 
                                SDL_WINDOWPOS_CENTERED, 
                                SDL_WINDOWPOS_CENTERED, 
                                1280, 
                                720, 
                                0);

    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }

    u32 rendflag = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, 3, rendflag);

    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
    
    f32 deltaTime = 0.016f;
    u64 startTime = 0;
    u64 endTime = 0;
    bool quitGame = false;

    LoadAssets(renderer);

    while(!quitGame)
    {
        startTime = SDL_GetPerformanceCounter();

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                quitGame = true;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quitGame = true;
                }
                break;
            }
            HandleEvent(&event);
        }   

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
 
        Update(deltaTime);
        Render(renderer);        

        SDL_RenderPresent(renderer);

        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;
        
        if(deltaTime > 0.016f)
        {
            deltaTime = 0.016f;
        }
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}