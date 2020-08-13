#include "gamezero.h"
#include "..\Engine\src\MicroEngine.c"

#include "PlayerData.h"
#include "EnemyData.h"

#define MUI_ORIGIN_ID 1020

global MUI ui;
global MUI_Input uiInput;

global MicroECSWorld ecsWorld;
global Entity player;

global Vector2 gravity = {0.0f, 200.0f};

global SDL_Texture *tileMapTexture = NULL;

char entityCountString[50] = "Entity_Count";

global u32 score = 0;
char scoreText[25] = "0";

void ShowMainMenu(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    bool quitMainMenu = false;
    
    while(!quitMainMenu)
    {
        while(SDL_PollEvent(event))
        {
            switch(event->type)
            {
				case SDL_QUIT:
                quitMainMenu = true;
                *gameMode = GAME_EXIT;
                break;
            }
            MUI_GetInput(&uiInput, event);
        }
        
        MUI_BeginFrame(&ui, &uiInput);
        
        //game title
        MUI_Rect gameTitleRect = ME_RectInit(SCREEN_WIDTH / 2, 200, 100, 30);
        MUI_Text(&ui, GEN_MUI_ID(), gameTitleRect, "Game Zero", 80);
        
        //menu buttons
		MUI_Rect rect = ME_RectInit(1280 / 2, 350, 300, 40);
        MUI_PushColumnLayout(&ui, rect, 15);
        {
            if(MUI_ButtonA(&ui,GEN_MUI_ID(), "Play"))
            {
                quitMainMenu = true;
                *gameMode = GAME_PLAY;
            }
            
            if(MUI_ButtonA(&ui,GEN_MUI_ID(), "Credits"))
            {
                quitMainMenu = true;
                *gameMode = GAME_CREDITS;
            }
            
            if(MUI_ButtonA(&ui,GEN_MUI_ID(), "Quit"))
            {
                quitMainMenu = true;
                *gameMode = GAME_EXIT;
            }
        }
        
        MUI_PopLayout(&ui);
        
        SDL_SetRenderDrawColor(renderer, 34, 177, 76, 0);
        SDL_RenderClear(renderer);
        
        MUI_EndFrame(&ui, renderer);
        
        SDL_RenderPresent(renderer);
    }
}

void ShowCredits(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    bool quitCredits = false;
    while(!quitCredits)
    {
        while(SDL_PollEvent(event))
        {
            switch(event->type)
            {
				case SDL_QUIT:
                quitCredits = true;
                *gameMode = GAME_EXIT;
                break;
            }
            MUI_GetInput(&uiInput, event);
        }
        
        MUI_BeginFrame(&ui, &uiInput);
        
        //game credits
        MUI_Rect gameCreditRect = ME_RectInit(SCREEN_WIDTH / 2, 300, 100, 30);
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Developed by LoneCoder", 30);
        
        //go back to menu button
		MUI_Rect rect = ME_RectInit(1280 / 2, 600, 300, 40);
        if(MUI_Button(&ui, GEN_MUI_ID(),"Go Back To Main Menu", rect))
        {
            quitCredits = true;
            *gameMode = GAME_MAIN_MENU;
        }
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 0);
        SDL_RenderClear(renderer);
        MUI_EndFrame(&ui, renderer);
        SDL_RenderPresent(renderer);
    }
}

void HandleEvent(SDL_Event *event)
{
    MUI_GetInput(&uiInput, event);
    PlayerInputSystem(&ecsWorld.input, event);
}

void GameUpdateAndRender(f32 deltaTime, SDL_Renderer *renderer)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    sprintf(entityCountString, "ENTITY_COUNT : %d", ecsWorld.activeEntityCount);
    MUI_Rect textRect = ME_RectInit(120, 20, 120, 300);
    MUI_Text(&ui, GEN_MUI_ID(), textRect, entityCountString, 14);
    
    sprintf(scoreText, "SCORE : %d", score);
    MUI_Rect scoreTextRect = ME_RectInit(400, 20, 120, 30);
    MUI_Text(&ui, GEN_MUI_ID(), scoreTextRect, scoreText, 14);
    
    PhysicsSystem(&ecsWorld, gravity, 0.016f);
    
    u32 i = 0;
    for(i = 0; i < ecsWorld.activeEntityCount; i++)
    {
        if (!IsEntityDead(i, &ecsWorld))
        {
            if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], AnimationSystemSignature))
            {
                AnimationSystem(&ecsWorld.animations[i]);
            }
            
            switch (ecsWorld.tags[i])
            {
                case ENTITY_TAG_PLAYER:
                PlayerControlSystem(&ecsWorld.transforms[i],
									&ecsWorld.animations[i],
									&ecsWorld.input,
									&ecsWorld.physics[i],
									&ecsWorld.stat[i]);
                
                FiringSystem(&ecsWorld, &ecsWorld.input,
							 &ecsWorld.stat[i],
							 ecsWorld.transforms[i].position);
                
                break;
                
                case ENTITY_TAG_LIZARD:
                if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
											  TRANSFORM_COMPONENT_SIGN |
											  ANIMATION_COMPONENT_SIGN |
											  ENTITYSTAT_COMPONENT_SIGN))
                {
                    EnemyPatrolSystem(&ecsWorld.transforms[i],
									  &ecsWorld.stat[i],
									  &ecsWorld.animations[i],
                                      deltaTime);
                    
                    Vector2 pos = ecsWorld.transforms[i].position;
                    MUI_Rect rect = ME_RectInit(pos.x, pos.y - 50, 60, 8);
                    ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, rect);
                }
                
                if(ecsWorld.physics[i].collided && ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_BULLET)
                {
                    ecsWorld.stat[i].EnemyStat.health -= 0.5f;
                }
                
                if(ecsWorld.stat[i].EnemyStat.health < 0.1)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                    score += 10;
                }
                break;
                
                case ENTITY_TAG_BULLET:
                ecsWorld.renders[i].texture = lizardRender.texture;
                
                if(ecsWorld.physics[i].collided)
                {
                    if(ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_PLATFORM ||
                       ecsWorld.physics[i].tagOfCollidedEntity == ENTITY_TAG_LIZARD)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                    }
                }
                break;
            }
        }
    }
    
    //Rendering
    {
        if(tileMapTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMapTexture, NULL, NULL);
        }
        
        u32 i = 0;
        loop(i, ecsWorld.activeEntityCount)
        {
            if(!IsEntityDead(i, &ecsWorld))
            {
                if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
                                              RenderSystemSignature))
                {
                    RenderSystem(&ecsWorld.transforms[i],
                                 &ecsWorld.animations[i],
                                 &ecsWorld.renders[i],
                                 renderer);
                }
                else if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
                                                   TRANSFORM_COMPONENT_SIGN |
                                                   RENDER_COMPONENT_SIGN))
                {
                    RenderSystemSimple(&ecsWorld.transforms[i],
                                       &ecsWorld.renders[i],
                                       renderer);
                }
                
#if 0
                //NOTE(abhicv): Drawing collision rect for debug
                if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
                                              PHYSICS_COMPONENT_SIGN))
                {
                    SDL_Color color = {255, 255, 255, 0};
                    SDL_Rect rect = {0};
                    CollisionRect cRect = ecsWorld.physics[i].physicsBody.rect;
                    rect.x = cRect.x - cRect.width / 2;
                    rect.y = cRect.y - cRect.height / 2;
                    rect.w = cRect.width;
                    rect.h = cRect.height;
                    ME_RenderDrawRect(renderer, &rect, color);
                }
#endif
            }
        }
        
        MUI_EndFrame(&ui, renderer);
    }
}

void LoadEntity(SDL_Renderer *renderer, const char *fileName)
{
    FILE *entityMapFile = fopen(fileName, "r");
    
    PhysicsComponent physics = {0};
    
    if(entityMapFile != NULL)
    {
        u32 entityCount = 0;
        fscanf(entityMapFile, "n:%d\n", &entityCount);
        
        u32 n = 0;
        for(n = 0; n < entityCount; n++)
        {
            u32 entityTag = ENTITY_TAG_NONE;
            i32 x = 0;
            i32 y = 0;
            
            fscanf(entityMapFile, "e:%d,%d,%d\n", &entityTag, &x, &y);
            
            switch (entityTag)
            {
                case ENTITY_TAG_PLAYER:
                
                transform.position = Vector2Init((f32)x, (f32)y);
                
                render.texture = IMG_LoadTexture(renderer, "assets/Sprites/Player.png");
                render.width = 64;
                render.height = 64;
                
                anim.animations[Idle] = idleAnim;
                anim.animations[Walking] = walkAnim;
                anim.animations[Jump] = jumpAnim;
                anim.animations[Attack] = shootAnim;
                
                physics.physicsBody = CreatePhysicsBody(transform.position, 5.0, 30, 64);
                physics.physicsBody.velocity = Vector2Init(0.0f, 0.0f);
                physics.physicsBody.affectedByGravity = true;
                physics.physicsBody.restitution = 0.0f;
                physics.physicsBody.friction = 0.05f;
                physics.excludeEntityTag = ENTITY_TAG_NONE;
                
                player = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_PLAYER);
                if (player < MAX_ENTITY_COUNT)
                {
                    ecsWorld.entitySignature[player] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                    
                    ecsWorld.transforms[player] = transform;
                    ecsWorld.renders[player] = render;
                    ecsWorld.animations[player] = anim;
                    ecsWorld.stat[player] = PlayerStat;
                    ecsWorld.physics[player] = physics;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                
                break;
                
                case ENTITY_TAG_LIZARD:
                
                lizardTransform.position = Vector2Init((f32)x, (f32)y);
                
                lizardAnimation.animations[Idle] = lizardIdleAnim;
                lizardAnimation.animations[Walking] = lizardWalkAnim;
                
                lizardRender.texture = IMG_LoadTexture(renderer, "assets/Sprites/enemies.png");
                lizardRender.width = 64;
                lizardRender.height = 64;
                
                lizardStat.EnemyStat.startPosition = lizardTransform.position;
                lizardStat.EnemyStat.moveRight = !!(n % 2); //odd lizard moves right
                lizardStat.EnemyStat.patrolDistance = 40.0f;
                
                physics.physicsBody = CreatePhysicsBody(lizardTransform.position, 2.0, 50, 64);
                physics.physicsBody.restitution = 0.0f;
                physics.physicsBody.friction = 0.02f;
                physics.excludeEntityTag = ENTITY_TAG_NONE;
                
                Entity lizard = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_LIZARD);
                if (lizard < MAX_ENTITY_COUNT)
                {
                    ecsWorld.entitySignature[lizard] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                    
                    ecsWorld.transforms[lizard] = lizardTransform;
                    ecsWorld.renders[lizard] = lizardRender;
                    ecsWorld.animations[lizard] = lizardAnimation;
                    ecsWorld.stat[lizard] = lizardStat;
                    ecsWorld.physics[lizard] = physics;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                break;
                
                default:
                break;
            }
        }
        fclose(entityMapFile);
    }
}

//NOTE(abhicv): Loading tilemap as a texture
SDL_Texture *LoadTileMapTexture(SDL_Renderer *renderer, const char *fileName)
{
    TileMap tileMap = ME_LoadTileMap(fileName);
    
    u32 width = tileMap.tileMapWidth * tileMap.tileWidth;
    u32 height = tileMap.tileMapHeight * tileMap.tileHeight;
    
    SDL_Surface *tileSheetSurface = IMG_Load("assets/Sprites/tiles.png");
    SDL_Surface *tileSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    
    u32 i = 0;
    for(i = 0; i < tileMap.tileCount; i++)
    {
        SDL_Rect destRect = {0};
        destRect.x = tileMap.tiles[i].tileMapPosX;
        destRect.y = tileMap.tiles[i].tileMapPosY;
        destRect.w = tileMap.tileWidth;
        destRect.h = tileMap.tileHeight;
        
        SDL_Rect srcRect = {0};
        srcRect.x = tileMap.tiles[i].tileSheetPosX;
        srcRect.y = tileMap.tiles[i].tileSheetPosY;
        srcRect.w = tileMap.tileSheetTileWidth;
        srcRect.h = tileMap.tileSheetTileHeight;
        
        SDL_BlitScaled(tileSheetSurface, &srcRect, tileSurface, &destRect);
    }
    
    SDL_Texture *tileMapTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
    
    SDL_FreeSurface(tileSheetSurface);
    SDL_FreeSurface(tileSurface);
    
    return tileMapTexture;
}

void LoadData(SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    //cleaning while reloading
    {
        ecsWorld.activeEntityCount = 0;
        SDL_DestroyTexture(tileMapTexture);
        tileMapTexture = NULL;
        
        u32 n = 0;
        for(n = 0; n < ecsWorld.activeEntityCount; n++)
        {
            SDL_DestroyTexture(ecsWorld.renders[n].texture);
            ecsWorld.renders[n].texture = NULL;
        }
    }
    
    LoadEntity(renderer, "data/levels/level.emap");
    
    tileMapTexture = LoadTileMapTexture(renderer, "data/levels/level.tmap");
    
    //platforms
    {
        TransformComponent transform = {0};
        RenderComponent render = {0};
        PhysicsComponent physics = {0};
        
        FILE *colMapFile = fopen("data/levels/level.cmap", "r");
        if(colMapFile != NULL)
        {
            u32 platformCount = 0;
            fscanf(colMapFile, "n:%d\n", &platformCount);
            
            u32 n = 0;
            for(n = 0; n < platformCount; n++)
            {
                Entity platform = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_PLATFORM);
                if (platform < MAX_ENTITY_COUNT)
                {
                    ecsWorld.entitySignature[platform] = TRANSFORM_COMPONENT_SIGN | PHYSICS_COMPONENT_SIGN;
                    
                    i32 x = 0;
                    i32 y = 0;
                    u32 w = 0;
                    u32 h = 0;
                    
                    fscanf(colMapFile, "c:%d,%d,%d,%d\n", &x, &y, &w, &h);
                    
                    transform.position = Vector2Init(x + w / 2, y + h / 2);
                    render.width = w;
                    render.height = h;
                    render.texture = NULL;
                    
                    physics.physicsBody = CreatePhysicsBody(transform.position, 0.0f, render.width, render.height);
                    physics.physicsBody.restitution = 0.0f;
                    physics.physicsBody.affectedByGravity = false;
                    physics.excludeEntityTag = ENTITY_TAG_NONE;
                    
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
    }
}