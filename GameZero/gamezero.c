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

global Level levels[] = {
    LEVEL_MAP(level06),
    LEVEL_MAP(level00),
    LEVEL_MAP(level01),
    LEVEL_MAP(level02),
    LEVEL_MAP(level03),
    LEVEL_MAP(level04),
    LEVEL_MAP(level05),
};

global MUI ui;
global MUI_Input uiInput;
global MicroECSWorld ecsWorld;
global GameResource gameResource;

//used to address the player from anywhere
global Entity player;
global Vector2 gravity = {0.0f, 100.0f};
global u32 coinCount = 0;
global bool levelCompleted = false;
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
        
        // game title
        MUI_Rect gameTitleRect = ME_RectInit(40, 200, 100, 30);
        MUI_Text(&ui, GEN_MUI_ID(), gameTitleRect, "Knight At The Castle", 80, textStyle, MUI_TEXT_ALIGN_LEFT);
        
        // menu buttons
        MUI_Rect rect = ME_RectInit(40, 300, 200, 50);
        MUI_PushColumnLayout(&ui, rect, 15);
        {
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Start", buttonStyle))
            {
                quitMainMenu = true;
                *gameMode = GAME_LOAD_LEVEL;
            }
            
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Credits", buttonStyle))
            {
                quitMainMenu = true;
                *gameMode = GAME_CREDITS;
            }
            
            if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Exit", buttonStyle))
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
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Developed by : Abhijith C V", 30, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        gameCreditRect.y += 80;
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Play testers : Javid and Ebin", 30, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        
        //go to menu button
        MUI_Rect rect = ME_RectInit(1280 / 2, 600, 300, 45);
        if(MUI_Button(&ui, GEN_MUI_ID(),"back", rect, buttonStyle))
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
        
    if(levelCompleted) 
    {
        MUI_Rect rect = {0};
        rect.width = 100;
        rect.height = 50;
        rect.x = (SCREEN_WIDTH / 2) - (rect.width / 2);
        rect.y = 100;    
        MUI_Text(&ui, GEN_MUI_ID(), rect, "Level Completed!", 50, textStyle, MUI_TEXT_ALIGN_MIDDLE);
    }

    // PhysicsSystem(&ecsWorld, gravity, deltaTime);
    PhysicsSystem(&ecsWorld, gravity, 0.016f);
    
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

                if (ecsWorld.physics[i].collided)
                {
                    for(int n = 0; n < ecsWorld.physics[i].collisionCount; n++)
                    {
                        if(ecsWorld.physics[i].normals[n].y == 1.0f)
                        {
                            ecsWorld.physics[i].isGrounded = true;
                            break;
                        }
                    }
                    
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & (ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_FLYEE | ENTITY_TAG_ENEMY_BULLET);
                    
                    if (masked == ENTITY_TAG_LIZARD || masked == ENTITY_TAG_SLIME ||
                        masked == ENTITY_TAG_FLYEE || masked == ENTITY_TAG_ENEMY_BULLET)
                    {
                        // player dies
                    }                    
                }
                
                PlayerControlSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i], &ecsWorld.input, &ecsWorld.physics[i], &ecsWorld.states[i]);
                
                FiringSystem(&ecsWorld, &ecsWorld.input, ecsWorld.states[i].Player.facingDir, ecsWorld.transforms[i].position);
                
                if(ecsWorld.physics[i].physicsBody.velocity.y > 0.0f) // when coming down after jump
                {
                    ecsWorld.physics[i].physicsBody.velocity.y += 30.0f * gravity.y * deltaTime;
                }
                else if (ecsWorld.physics[i].physicsBody.velocity.y < 0.0f && !ecsWorld.input.jumpKeyHeld) // for single jump key press
                {
                    ecsWorld.physics[i].physicsBody.velocity.y += 10.0f * gravity.y * deltaTime;
                }
                break;
                
                case ENTITY_TAG_LIZARD:
                case ENTITY_TAG_SLIME:
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | ENTITYSTAT_COMPONENT_SIGN))
                {
                    EnemyPatrolSystem(&ecsWorld.transforms[i], &ecsWorld.states[i], &ecsWorld.animations[i], deltaTime);                    
                }
                
                if(ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_BULLET;
                    if (masked == ENTITY_TAG_BULLET)
                    {
                        ecsWorld.states[i].Enemy.health -= 0.2f;
                    }
                }
                
                if(ecsWorld.states[i].Enemy.health < 0.1f)
                {
                    ecsWorld.entityDeathFlag[i] = true;
                }                
                break;
                
                case ENTITY_TAG_FLYEE:
                FlyeeAISystem(&ecsWorld, &ecsWorld.states[i],
                              &ecsWorld.transforms[i], &ecsWorld.animations[i],
                              ecsWorld.transforms[player].position, deltaTime);
                                
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_BULLET;
                    if(masked == ENTITY_TAG_BULLET)
                    {
                        ecsWorld.states[i].Enemy.health -= 0.2f;
                    }
                }
                
                if (ecsWorld.states[i].Enemy.health < 0.1f)
                {
                    ecsWorld.entityDeathFlag[i] = true;
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
                
                // killing bullet after travelling a distance
                Vector2 bullet = Vector2Subtract(ecsWorld.transforms[i].position, ecsWorld.states[i].Bullet.startPosition);
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
                
                // oscillating coin
                ecsWorld.transforms[i].position.y += (0.2f * sinf(SDL_GetTicks() / 100.0f));
                break;

                case ENTITY_TAG_CAPTURE_FLAG:
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_PLAYER;
                    if (masked == ENTITY_TAG_PLAYER)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                        levelCompleted = true;
                    }
                }
                break;
            }
        }
    }
    
    // for only single key press
    ecsWorld.input.leftCtrlKeyDown = false;
    ecsWorld.input.jumpKeyDown = false;
    
    // Rendering
    {
        // tile map texture
        SDL_Texture *tileMapTexture = levels[currentLevelIndex].tileMapTexture;
        SDL_Rect rect = {0};
        SDL_QueryTexture(tileMapTexture, 0, 0, &rect.w, &rect.h);
        
        if (tileMapTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMapTexture, 0, &rect);
        }
        
        // entities
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
                // drawing collision rects for visual debug
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

                    if(ecsWorld.tags[i] == ENTITY_TAG_PLAYER) 
                    {                        
                        SDL_Rect xVector = {0};
                        xVector.x = ecsWorld.physics[i].physicsBody.position.x;
                        xVector.y = ecsWorld.physics[i].physicsBody.position.y;
                        xVector.w = (ecsWorld.physics[i].physicsBody.velocity.x / PLAYER_MAX_SPEED) * 100.0f;
                        xVector.h = 3;
                        
                        ME_RenderFillRect(renderer, &xVector, (SDL_Color){255, 0, 0, 0});
                        
                        SDL_Rect yVector = {0};
                        yVector.x = ecsWorld.physics[i].physicsBody.position.x;
                        yVector.y = ecsWorld.physics[i].physicsBody.position.y;
                        yVector.h = (ecsWorld.physics[i].physicsBody.velocity.y / PLAYER_JUMP_SPEED) * 100.0f;
                        yVector.w = 3;
                        ME_RenderFillRect(renderer, &yVector, (SDL_Color){0, 0, 255, 0});
                    }

                }
#endif
            }
        }

        MUI_EndFrame(&ui, renderer);
    }
}

// all required assets are loaded once at the beginning
// TODO: pack all the game resource into one file and load at beginning
GameResource LoadGameResource(SDL_Renderer *renderer)
{
    GameResource gameResource = {0};
    
    gameResource.playerSprite = IMG_LoadTexture(renderer, "data/graphics/player.png");
    gameResource.enemySprite = IMG_LoadTexture(renderer, "data/graphics/enemies.png");
    gameResource.itemSprite = IMG_LoadTexture(renderer, "data/graphics/items.png");
    gameResource.tileSheetSurface = IMG_Load("data/graphics/tiles.png");
    
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

    if (entityMapFile == NULL)
    {
        printf("Failed to load file: '%s'\n", fileName);
        return;
    }

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

            case ENTITY_TAG_CAPTURE_FLAG:
            LoadCaptureFlag(&ecsWorld, &gameResource, x, y);
            break;
        }
    }
    fclose(entityMapFile);
}

// Loading tilemap as a big texture
SDL_Texture *LoadTileMapTexture(SDL_Renderer *renderer, const char *fileName)
{
    printf("Loading tile map as texture from '%s'\n", fileName);
    TileMap tileMap = ME_LoadTileMap(fileName);

    u32 width = tileMap.tileMapWidth * tileMap.tileWidth;
    u32 height = tileMap.tileMapHeight * tileMap.tileHeight;
    
    SDL_Surface *tileSheetSurface = gameResource.tileSheetSurface;
    SDL_Surface *tileSurface = SDL_CreateRGBSurface(0, width, height, 24, 0, 0, 0, 0);

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
    
    SDL_Texture *tileMapTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
    SDL_FreeSurface(tileSurface);
    free(tileMap.tiles);
    
    return tileMapTexture;
}

void LoadLevelCollisionGeometry(const char *collisionMapFile)
{
    printf("Loading collision geometry from '%s'\n", collisionMapFile);

    FILE *colMapFile = fopen(collisionMapFile, "r");

    if(colMapFile == NULL)
    {
        printf("Failed to load file: '%s'\n", collisionMapFile);
        return;
    }

    u32 platformCount = 0;
    fscanf(colMapFile, "n:%d\n", &platformCount);

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
    fclose(colMapFile);
}

void LoadData(SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    if(!gameResource.loaded) gameResource = LoadGameResource(renderer);

    // reseting data
    SDL_DestroyTexture(levels[currentLevelIndex].tileMapTexture);
    levels[currentLevelIndex].tileMapTexture = 0;
    
    memset(&ecsWorld, 0, sizeof(MicroECSWorld));
    levelCompleted = false;
    currentLevelIndex = nextLevelIndex;
    
    printf("Loading level %d data ...\n", currentLevelIndex);
    
    // load entity
    LoadEntity(levels[currentLevelIndex].entityMapFile);
    
    // load level as texture
    levels[currentLevelIndex].tileMapTexture = LoadTileMapTexture(renderer, levels[currentLevelIndex].tileMapFile);
    
    // load collision geometry of the level
    LoadLevelCollisionGeometry(levels[currentLevelIndex].collisionMapFile);
    
    //moving to next level
    u32 levelCount = sizeof(levels) / sizeof(Level);
    nextLevelIndex = (nextLevelIndex <  (levelCount - 1)) ? nextLevelIndex + 1 : 0;
}

// for loading game data from editor
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