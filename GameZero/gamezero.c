#include "..\Engine\src\MicroEngine.c"
#include "system.c"
#include "load_data.c"

#include "gamezero.h"
#include "data.h"
#include "ui_style.h"

#define MATCH_TAG(colTag, entityTag) (colTag & entityTag)
#define SQUARE(x) x*x

global u32 currentLevelIndex = 0;
global u32 nextLevelIndex = 0;

#ifndef __EMSCRIPTEN__
global Level levels[] = {
    LEVEL_MAP(default),
    LEVEL_MAP(level00),
    LEVEL_MAP(level01),
    LEVEL_MAP(level02),
    LEVEL_MAP(level03),
    LEVEL_MAP(level04),
    LEVEL_MAP(level05),
};
#else
global Level levels[] = {
    [0] = {
        .entityMapFile = "bin/data/levels/level00.emap",
        .tileMapFile = "bin/data/levels/level00.tmap",
        .collisionMapFile = "bin/data/levels/level00.cmap",
        .tileMapTexture = 0,
    },
};
#endif

global MUI ui;
global MUI_Input uiInput;
global MicroECSWorld ecsWorld;
global GameResource gameResource;

//used to address the player from anywhere
global Entity player;
global Vector2 gravity = {0.0f, 100.0f};
global u32 score = 0;
global f32 playerHealth = 1.0f;
global u32 coinCount = 0;

global b32 debugDraw = false;

void ShowMainMenu(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    currentLevelIndex = 0;
    nextLevelIndex = 0;
    
    bool quitMainMenu = false;
    
    while(!quitMainMenu)
    {
        if(SDL_WaitEvent(event))
        {
            switch(event->type)
            {
                case SDL_QUIT:
                quitMainMenu = true;
                *gameMode = GAME_EXIT;
                break;
                
                case SDL_KEYDOWN:
                if(event->key.keysym.sym == SDLK_ESCAPE)
                {
                    quitMainMenu = true;
                    *gameMode = GAME_EXIT;
                }
                break;
            }
            MUI_GetInput(&uiInput, event);
        }
        
        MUI_BeginFrame(&ui, &uiInput);
        
        //game title
        MUI_Rect gameTitleRect = ME_RectInit(SCREEN_WIDTH / 2, 200, 100, 30);
        MUI_Text(&ui, GEN_MUI_ID(), gameTitleRect, "gamezero", 60, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        
        //menu buttons
        MUI_Rect rect = ME_RectInit(1280 / 2, 350, 300, 45);
        MUI_PushColumnLayout(&ui, rect, 15);
        {
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "PLAY", buttonStyle))
            {
                quitMainMenu = true;
                *gameMode = GAME_LOAD_LEVEL;
            }
            
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "CREDITS", buttonStyle))
            {
                quitMainMenu = true;
                *gameMode = GAME_CREDITS;
            }
            
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "EXIT", buttonStyle))
            {
                quitMainMenu = true;
                *gameMode = GAME_EXIT;
            }
        }
        MUI_PopLayout(&ui);
        
        SDL_SetRenderDrawColor(renderer, 0, 42, 62, 0);
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
                
                case SDL_KEYDOWN:
                if(event->key.keysym.sym == SDLK_ESCAPE)
                {
                    quitCredits = true;
                    *gameMode = GAME_EXIT;
                }
                break;
            }
            MUI_GetInput(&uiInput, event);
        }
        
        MUI_BeginFrame(&ui, &uiInput);
        
        //game credits text
        MUI_Rect gameCreditRect = ME_RectInit(SCREEN_WIDTH / 2, 250, 100, 30);
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Developed by : loneCoder", 30, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        gameCreditRect.y += 80;
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Game Testers : Cube and Jayee", 30, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        
        //go to menu button
        MUI_Rect rect = ME_RectInit(1280 / 2, 600, 300, 45);
        if(MUI_Button(&ui, GEN_MUI_ID(),"GOTO MAIN MENU", rect, buttonStyle))
        {
            quitCredits = true;
            *gameMode = GAME_MAIN_MENU;
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 42, 62, 0);
        SDL_RenderClear(renderer);
        MUI_EndFrame(&ui, renderer);
        SDL_RenderPresent(renderer);
    }
}

void HandleEvent(SDL_Event *event)
{
    MUI_GetInput(&uiInput, event);
    PlayerInputSystem(&ecsWorld.input, event);
    
    switch(event->type)
    {
        case SDL_KEYDOWN:
        if(event->key.keysym.sym == SDLK_f)
        {
            debugDraw = !debugDraw;
        }
        break;
    }
}

void GameUpdateAndRender(u32 *gameMode, bool *quitGame, f32 deltaTime, SDL_Renderer *renderer)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    MUI_Rect textRect = {0};
    textRect = ME_RectInit(20, 10, 120, 35);
    
    MUI_PushRowLayout(&ui, textRect, 120);
    {
        u8 entityCountString[128] = {0};
        sprintf(entityCountString, "Entity_Count : %d\0", ecsWorld.activeEntityCount);
        MUI_TextA(&ui, GEN_MUI_ID(), entityCountString, 20, textStyle, MUI_TEXT_ALIGN_LEFT);
        
        u8 scoreText[128] = {0};
        sprintf(scoreText, "Score : %d\0", score);
        MUI_TextA(&ui, GEN_MUI_ID(), scoreText, 20, textStyle, MUI_TEXT_ALIGN_LEFT);
        
        u8 coinCountText[128] = {0};
        sprintf(coinCountText, "Coins : %d\0", coinCount);
        MUI_TextA(&ui, GEN_MUI_ID(), coinCountText, 20, textStyle, MUI_TEXT_ALIGN_LEFT);
        
        playerHealth = MUI_SliderA(&ui, GEN_MUI_ID(), playerHealth, sliderStyle);
    }
    MUI_PopLayout(&ui);
    
    //PhysicsSystem(&ecsWorld, gravity, 0.016f);
    PhysicsSystem(&ecsWorld, gravity, deltaTime);
    
    //NOTE(abhicv): looping through all the active entity in the world
    for(u32 i = 0; i < ecsWorld.activeEntityCount; i++)
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
                
                //ecsWorld.physics[i].isGrounded = true;
                if (ecsWorld.physics[i].collided)
                {
                    if(ecsWorld.physics[i].physicsBody.collisionNormal.y == 1.0f)
                    {
                        ecsWorld.physics[i].isGrounded = true;
                    }
                    
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & (ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_FLYEE | ENTITY_TAG_ENEMY_BULLET);
                    
                    if (masked == ENTITY_TAG_LIZARD || masked == ENTITY_TAG_SLIME ||
                        masked == ENTITY_TAG_FLYEE || masked == ENTITY_TAG_ENEMY_BULLET)
                    {
                        if(ecsWorld.stat[i].PlayerStat.affectHealthDelay > 2.0f)
                        {
                            playerHealth = playerHealth - 0.1f;
                            ecsWorld.stat[i].PlayerStat.affectHealthDelay = 0;
                        }
                    }
                    
                    ecsWorld.stat[i].PlayerStat.affectHealthDelay += deltaTime;
                }
                
                PlayerControlSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i],
                                    &ecsWorld.input, &ecsWorld.physics[i],
                                    &ecsWorld.stat[i]);
                
                FiringSystem(&ecsWorld, &ecsWorld.input, &ecsWorld.stat[i], ecsWorld.transforms[i].position);
                
                //altering player jump gravity for best jump response
                if(ecsWorld.physics[i].physicsBody.velocity.y > 0.0f) //when comming down after jump
                {
                    ecsWorld.physics[i].physicsBody.velocity.y += (15.0f - 1.0f) * gravity.y * deltaTime;
                }
                else if(ecsWorld.physics[i].physicsBody.velocity.y < 0.0f && !ecsWorld.input.jumpKeyHeld) //for single jump key press
                {
                    ecsWorld.physics[i].physicsBody.velocity.y += (5.0f - 1.0f) * gravity.y * deltaTime;
                }
                break;
                
                case ENTITY_TAG_LIZARD:
                case ENTITY_TAG_SLIME:
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | ENTITYSTAT_COMPONENT_SIGN))
                {
                    EnemyPatrolSystem(&ecsWorld.transforms[i], &ecsWorld.stat[i], &ecsWorld.animations[i], deltaTime);
                    
                    Vector2 pos = ecsWorld.transforms[i].position;
                    MUI_Rect rect = ME_RectInit(pos.x, pos.y - 50, 60, 8);
                    ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, rect, sliderStyle);
                }
                
                if(ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_BULLET;
                    if (masked == ENTITY_TAG_BULLET)
                    {
                        ecsWorld.stat[i].EnemyStat.health -= 0.2f;
                    }
                }
                
                if(ecsWorld.stat[i].EnemyStat.health < 0.1f)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                    score += 10;
                }
                
                break;
                
                case ENTITY_TAG_FLYEE:
                FlyeeAISystem(&ecsWorld, &ecsWorld.stat[i],
                              &ecsWorld.transforms[i], &ecsWorld.animations[i],
                              ecsWorld.transforms[player].position, deltaTime);
                
                //Health bar
                Vector2 pos = ecsWorld.transforms[i].position;
                MUI_Rect rect = ME_RectInit(pos.x, pos.y - 50, 60, 8);
                ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, rect, sliderStyle);
                
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_BULLET;
                    if(masked == ENTITY_TAG_BULLET)
                    {
                        ecsWorld.stat[i].EnemyStat.health -= 0.2f;
                    }
                }
                
                if (ecsWorld.stat[i].EnemyStat.health < 0.1f)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                    score += 10;
                }
                break;
                
                case ENTITY_TAG_BULLET:
                ecsWorld.renders[i].texture = gameResource.itemSprite;
                
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & (ENTITY_TAG_PLATFORM | ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_FLYEE);
                    
                    if (masked == ENTITY_TAG_PLATFORM || masked == ENTITY_TAG_LIZARD || masked == ENTITY_TAG_SLIME || masked == ENTITY_TAG_FLYEE)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                    }
                }
                
                //NOTE(abhicv): limiting bullet travel distance
                Vector2 bullet = Vector2Subtract(ecsWorld.transforms[i].position, ecsWorld.stat[i].BulletStat.startPosition);
                if (Vector2SqrMag(bullet) > SQUARE(400.0f))
                {
                    ecsWorld.entityDeathFlag[i] = true;
                }
                break;
                
                case ENTITY_TAG_ENEMY_BULLET:
                ecsWorld.renders[i].texture = gameResource.itemSprite;
                
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & (ENTITY_TAG_PLATFORM | ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_PLAYER);
                    
                    if (masked == ENTITY_TAG_PLATFORM || masked == ENTITY_TAG_LIZARD || masked == ENTITY_TAG_SLIME || masked == ENTITY_TAG_PLAYER)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                    }
                }
                break;
                
                case ENTITY_TAG_COIN:
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_PLAYER;
                    if (masked == ENTITY_TAG_PLAYER)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                        coinCount += 1;
                    }
                }
                
                //oscillating coin
                ecsWorld.transforms[i].position.y += (0.4f * sinf(SDL_GetTicks() / 200.0f));
                break;
            }
        }
    }
    
    //for only single key press
    ecsWorld.input.leftCtrlKeyDown = false;
    ecsWorld.input.jumpKeyDown = false;
    
    //Rendering
    {
        //Render tile map texture
        SDL_Texture *tileMapTexture = levels[currentLevelIndex].tileMapTexture;
        SDL_Rect rect = {0};
        SDL_QueryTexture(tileMapTexture, NULL, NULL, &rect.w, &rect.h);
        
        //printf("tileMapTexture: %d\n", tileMapTexture);
        
        if (tileMapTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMapTexture, NULL, &rect);
        }
        
        //Rendering entities
        for(u32 i = 0; i < ecsWorld.activeEntityCount; i++)
        {
            if (!IsEntityDead(i, &ecsWorld))
            {
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], RenderSystemSignature))
                {
                    RenderSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i], &ecsWorld.renders[i], renderer);
                }
                else if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | RENDER_COMPONENT_SIGN))
                {
                    RenderSystemSimple(&ecsWorld.transforms[i], &ecsWorld.renders[i], renderer);
                }
                
#if 1
                
                //NOTE(abhicv): Drawing collision rect for visual debug
                if(debugDraw)
                {
                    if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], PHYSICS_COMPONENT_SIGN))
                    {
                        SDL_Color color = {255, 255, 255, 0};
                        if(ecsWorld.physics[i].collided)
                        {
                            color = (SDL_Color){255, 0, 0, 0};
                        }
                        CollisionRect cRect = ecsWorld.physics[i].physicsBody.rect;
                        SDL_Rect rect = {0};
                        rect.x = cRect.x - cRect.width / 2;
                        rect.y = cRect.y - cRect.height / 2;
                        rect.w = cRect.width;
                        rect.h = cRect.height;
                        ME_RenderDrawRect(renderer, &rect, color);
                    }
                }
#endif
            }
        }
        MUI_EndFrame(&ui, renderer);
    }
}

//NOTE(abhicv): all required assets are loaded once at the beginning
//TODO(abhicv): pack all the game resource into one file and load at beginning
GameResource LoadGameResource(SDL_Renderer *renderer)
{
    GameResource gameResource = {0};
    
    gameResource.playerSprite =  IMG_LoadTexture(renderer, "assets/Sprites/Player.png");
    gameResource.enemySprite =  IMG_LoadTexture(renderer, "assets/Sprites/enemies.png");
    gameResource.itemSprite = IMG_LoadTexture(renderer, "assets/Sprites/items.png");
    
    gameResource.tileSheetSurface =  IMG_Load("assets/Sprites/tiles.png");
    
    gameResource.coinSound = ME_LoadAudio("data/audio/coin_collect.wav");
    gameResource.shootSound = ME_LoadAudio("data/audio/shoot.wav");
    gameResource.jumpSound = ME_LoadAudio("data/audio/jump.wav");
    gameResource.hurtSound = ME_LoadAudio("data/audio/hurt.wav");
    
    gameResource.loaded = true;
    
    return gameResource;
}

void LoadEntity(const char *fileName)
{
    FILE *entityMapFile = fopen(fileName, "r");
    
    if (entityMapFile != NULL)
    {
        u32 entityCount = 0;
        fscanf(entityMapFile, "n:%d\n", &entityCount);
        
        if(entityCount > MAX_ENTITY_COUNT)
        {
            printf("ECS World cannot store %d entities!! Entity Capacity: %d\n", entityCount, MAX_ENTITY_COUNT);
            return;
        }
        
        printf("Loading entities from '%s'\n", fileName);
        
        for(u32 n = 0; n < entityCount; n++)
        {
            u32 entityTag = ENTITY_TAG_NONE;
            i32 x = 0, y = 0;
            fscanf(entityMapFile, "e:%d,%d,%d\n", &entityTag, &x, &y);
            
            switch (entityTag)
            {
                case ENTITY_TAG_PLAYER:
                player = LoadPlayer(&ecsWorld, &gameResource, x, y);
                break;
                
                case ENTITY_TAG_LIZARD:
                LoadLizard(&ecsWorld, &gameResource, x, y);
                break;
                
                case ENTITY_TAG_SLIME:
                LoadSlime(&ecsWorld, &gameResource, x, y);
                break;
                
                case ENTITY_TAG_FLYEE:
                LoadFlyee(&ecsWorld, &gameResource, x, y);
                break;
                
                case ENTITY_TAG_COIN:
                LoadCoin(&ecsWorld, &gameResource, x, y);
                break;
                
                case ENTITY_TAG_CRATE:
                LoadCrate(&ecsWorld, &gameResource, x, y);
                break;
            }
        }
        fclose(entityMapFile);
    }
}

//NOTE(abhicv): Loading tilemap as a big texture
SDL_Texture *LoadTileMapTexture(SDL_Renderer *renderer, const char *fileName)
{
    TileMap tileMap = ME_LoadTileMap(fileName);
    printf("Loading tile map as texture from '%s'\n", fileName);
    u32 width = tileMap.tileMapWidth * tileMap.tileWidth;
    u32 height = tileMap.tileMapHeight * tileMap.tileHeight;
    
    SDL_Surface *tileSheetSurface = gameResource.tileSheetSurface;
    
    SDL_Surface *tileSurface = SDL_CreateRGBSurface(0, width, height, 24, 0, 0, 0, 0);
    
    if (tileSheetSurface != NULL && tileSurface != NULL)
    {
        for(u32 i = 0; i < tileMap.tileCount; i++)
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
    }
    
    SDL_Texture *tileMapTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
    SDL_FreeSurface(tileSurface);
    free(tileMap.tiles);
    
    return tileMapTexture;
}

void LoadLevelCollisionGeometry(const char *collisionMapFile)
{
    FILE *colMapFile = fopen(collisionMapFile, "r");
    if(colMapFile != NULL)
    {
        u32 platformCount = 0;
        fscanf(colMapFile, "n:%d\n", &platformCount);
        printf("Loading collision geometry from '%s'\n", collisionMapFile);
        
        for(u32 n = 0; n < platformCount; n++)
        {
            Entity platform = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_PLATFORM);
            
            if(platform < MAX_ENTITY_COUNT)
            {
                TransformComponent transform = {0};
                PhysicsComponent physics = {0};
                
                i32 x = 0, y = 0;
                u32 w = 0, h = 0;
                
                fscanf(colMapFile, "c:%d,%d,%d,%d\n", &x, &y, &w, &h);
                
                transform.position = Vector2Init(x + (w / 2), y + (h / 2));
                
                physics.physicsBody = CreatePhysicsBody(transform.position, 0.0f, w, h);
                physics.physicsBody.restitution = 0.0f;
                physics.physicsBody.affectedByGravity = false;
                physics.excludeEntityTag = ENTITY_TAG_NONE;
                
                ecsWorld.entitySignature[platform] = TRANSFORM_COMPONENT_SIGN | PHYSICS_COMPONENT_SIGN;
                ecsWorld.transforms[platform] = transform;
                ecsWorld.physics[platform] = physics;
            }
            else
            {
                printf("\nECS world full!!!\n");
            }
        }
    }
    fclose(colMapFile);
}

void LoadData(SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    if(!gameResource.loaded)
    {
        gameResource = LoadGameResource(renderer);
    }
    
    //NOTE(abhicv): resetting data from last level
    SDL_DestroyTexture(levels[currentLevelIndex].tileMapTexture);
    levels[currentLevelIndex].tileMapTexture = 0;
    
    memset(&ecsWorld, 0, sizeof(MicroECSWorld));
    score = 0;
    coinCount = 0;
    
    currentLevelIndex = nextLevelIndex;
    
    printf("Loading level %d data ...\n", currentLevelIndex);
    
    //NOTE(abhicv): load entity
    LoadEntity(levels[currentLevelIndex].entityMapFile);
    
    //NOTE(abhicv): load level as texture
    levels[currentLevelIndex].tileMapTexture = LoadTileMapTexture(renderer, levels[currentLevelIndex].tileMapFile);
    
    //NOTE(abhicv): load collision geometry of the level
    LoadLevelCollisionGeometry(levels[currentLevelIndex].collisionMapFile);
    
    //moving to next level
    u32 levelCount = sizeof(levels) / sizeof(Level);
    nextLevelIndex = (nextLevelIndex <  (levelCount - 1)) ? nextLevelIndex + 1 : 0;
}

//for game testing from MicroEditor
void LoadLevelData(const char *entityMapFile, const char *tileMapFile,
                   const char *collisionMapFile, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    if(!gameResource.loaded)
    {
        gameResource = LoadGameResource(renderer);
    }
    
    LoadEntity(entityMapFile);
    levels[0].tileMapTexture = LoadTileMapTexture(renderer, tileMapFile);
    LoadLevelCollisionGeometry(collisionMapFile);
}