#include "..\Engine\src\MicroEngine.c"
#include "system.c"

#include "gamezero.h"
#include "data.h"

#define MUI_ORIGIN_ID 1020

global u32 currentLevelIndex = 0;
global u32 nextLevelIndex = 0;

global Level levels[] = {
    LEVEL_MAP(00),
    LEVEL_MAP(01),
    LEVEL_MAP(02),
    LEVEL_MAP(03),
};

global GameResource gameResource;
global GameState gameState;

global MUI ui;
global MUI_Input uiInput;
global MicroECSWorld ecsWorld;
global Entity player;

global Vector2 gravity = {0.0f, 200.0f};

global char entityCountString[50] = "EntityCount";

global u32 score = 0;
global char scoreText[25] = "0";

global u32 coinCount = 0;
global char coinCountText[25] = "0";

MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {250, 200, 15},
        .highlightColor = {200, 140, 10},
        .textColor = {0, 0, 0},
        .fontSize = 14,
    },
};

MUI_Style sliderStyle = {
    .sliderStyle = {
        .bgColor = {255, 255, 255},
        .sliderColor = {255, 0, 0},
    }
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

void ShowMainMenu(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    currentLevelIndex = 0;
    nextLevelIndex = 0;
    
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
        MUI_Text(&ui, GEN_MUI_ID(), gameTitleRect, "gamezero", 60, textStyle);
        
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
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Developed by : loneCoder", 30, textStyle);
        gameCreditRect.y += 80;
        MUI_Text(&ui, GEN_MUI_ID(), gameCreditRect, "Game Testers : Cube and Jayee", 30, textStyle);
        
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

void ShowLevelMenu(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(event))
        {
            switch(event->type)
            {
                case SDL_QUIT:
                quit = true;
                *gameMode = GAME_EXIT;
                break;
                
                case SDL_KEYDOWN:
                if(event->key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    *gameMode = GAME_PLAY;
                }
                break;
            }
            MUI_GetInput(&uiInput, event);
        }
        
        MUI_BeginFrame(&ui, &uiInput);
        
        //game credits text
        MUI_Rect rect = ME_RectInit(SCREEN_WIDTH / 2, 250, 300, 45);
        MUI_Text(&ui, GEN_MUI_ID(), rect, "Level Completed !", 30, textStyle);
        rect.y += 80;
        MUI_Text(&ui, GEN_MUI_ID(), rect, scoreText, 30, textStyle);
        
        //go to menu button
        rect.y += 100;
        if(MUI_Button(&ui, GEN_MUI_ID(),"Resume", rect, buttonStyle))
        {
            quit = true;
            *gameMode = GAME_PLAY;
        }
        
        rect.y += 55;
        if(MUI_Button(&ui, GEN_MUI_ID(),"Main Menu", rect, buttonStyle))
        {
            quit = true;
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
    PlayerInputSystem(&ecsWorld.input, event, &gameResource);
}

void GameUpdateAndRender(u32 *gameMode, bool *quitGame, f32 deltaTime, SDL_Renderer *renderer)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    sprintf(entityCountString, "ENTITY_COUNT : %d", ecsWorld.activeEntityCount);
    MUI_Rect textRect = ME_RectInit(120, 20, 120, 300);
    MUI_Text(&ui, GEN_MUI_ID(), textRect, entityCountString, 14, textStyle);
    
    sprintf(scoreText, "SCORE : %d", score);
    textRect= ME_RectInit(400, 20, 120, 30);
    MUI_Text(&ui, GEN_MUI_ID(), textRect, scoreText, 16, textStyle);
    
    sprintf(coinCountText, "COINS : %d", coinCount);
    textRect = ME_RectInit(600, 20, 120, 30);
    MUI_Text(&ui, GEN_MUI_ID(), textRect, coinCountText, 16, textStyle);
    
    PhysicsSystem(&ecsWorld, gravity, 0.016f);
    
    for(u32 i = 0; i < ecsWorld.activeEntityCount; i++)
    {
        if(!IsEntityDead(i, &ecsWorld))
        {
            if(MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], AnimationSystemSignature))
            {
                AnimationSystem(&ecsWorld.animations[i]);
            }
            
            switch(ecsWorld.tags[i])
            {
                case ENTITY_TAG_PLAYER:
                u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & (ENTITY_TAG_PLATFORM | ENTITY_TAG_LIZARD);
                
                if(masked == ENTITY_TAG_PLATFORM || masked == ENTITY_TAG_LIZARD)
                {
                    ecsWorld.physics[i].isGrounded = true;
                }
                ecsWorld.physics[i].isGrounded = true;
                
                PlayerControlSystem(&ecsWorld.transforms[i],
                                    &ecsWorld.animations[i],
                                    &ecsWorld.input,
                                    &ecsWorld.physics[i],
                                    &ecsWorld.stat[i]);
                
                FiringSystem(&ecsWorld, &ecsWorld.input, &ecsWorld.stat[i], ecsWorld.transforms[i].position);
                break;
                
                case ENTITY_TAG_LIZARD:
                case ENTITY_TAG_SLIME:
                case ENTITY_TAG_FLYEE:
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | ANIMATION_COMPONENT_SIGN | ENTITYSTAT_COMPONENT_SIGN))
                {
                    EnemyPatrolSystem(&ecsWorld.transforms[i], &ecsWorld.stat[i], &ecsWorld.animations[i], deltaTime);
                    
                    Vector2 pos = ecsWorld.transforms[i].position;
                    MUI_Rect rect = ME_RectInit(pos.x, pos.y - 50, 60, 8);
                    ecsWorld.stat[i].EnemyStat.health = MUI_Slider(&ui, GEN_MUI_IDi(i), ecsWorld.stat[i].EnemyStat.health, rect, sliderStyle);
                }
                
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_BULLET;
                    if(masked == ENTITY_TAG_BULLET)
                    {
                        ME_PlayAudio(&gameResource.hurtSound);
                        ecsWorld.stat[i].EnemyStat.health -= 0.2f;
                    }
                }
                
                if (ecsWorld.stat[i].EnemyStat.health < 0.1)
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
                    if(masked == ENTITY_TAG_PLATFORM || masked == ENTITY_TAG_LIZARD || masked == ENTITY_TAG_SLIME || masked == ENTITY_TAG_FLYEE)
                    {
                        ecsWorld.entityDeathFlag[i] = true;
                    }
                }
                
                //NOTE(abhicv): limiting bullet travel distance
                Vector2 bullet = Vector2Subtract(ecsWorld.transforms[i].position, ecsWorld.stat[i].BulletStat.startPosition);
                if (Vector2SqrMag(bullet) > (200.0f * 200.0f))
                {
                    ecsWorld.entityDeathFlag[i] = true;
                }
                
                break;
                
                case ENTITY_TAG_COIN:
                if (ecsWorld.physics[i].collided)
                {
                    u32 masked = ecsWorld.physics[i].tagOfCollidedEntity & ENTITY_TAG_PLAYER;
                    if (masked == ENTITY_TAG_PLAYER)
                    {
                        ME_PlayAudio(&gameResource.coinSound);
                        
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
        SDL_Texture *tileMapTexture = levels[currentLevelIndex].tileMapTexture;
        SDL_Rect rect = {0};
        SDL_QueryTexture(tileMapTexture, NULL, NULL, &rect.w, &rect.h);
        
        if (tileMapTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMapTexture, NULL, &rect);
        }
        
        for(u32 i = 0; i < ecsWorld.activeEntityCount; i++)
        {
            if (!IsEntityDead(i, &ecsWorld))
            {
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
                                               RenderSystemSignature))
                {
                    RenderSystem(&ecsWorld.transforms[i], &ecsWorld.animations[i], &ecsWorld.renders[i], renderer);
                }
                else if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i], TRANSFORM_COMPONENT_SIGN | RENDER_COMPONENT_SIGN))
                {
                    RenderSystemSimple(&ecsWorld.transforms[i], &ecsWorld.renders[i], renderer);
                }
                
#if 0
                //NOTE(abhicv): Drawing collision rect for debug
                if (MECS_EntitySignatureEquals(ecsWorld.entitySignature[i],
                                               PHYSICS_COMPONENT_SIGN))
                {
                    SDL_Color color = {255, 255, 0, 0};
                    CollisionRect cRect = ecsWorld.physics[i].physicsBody.rect;
                    SDL_Rect rect = {0};
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

GameResource LoadGameResource(SDL_Renderer *renderer)
{
    GameResource gameResource = {0};
    
    gameResource.playerSprite =  IMG_LoadTexture(renderer, "assets/Sprites/Player.png");
    gameResource.enemySprite =  IMG_LoadTexture(renderer, "assets/Sprites/enemies.png");
    gameResource.tileSheetSurface =  IMG_Load("assets/Sprites/tiles.png");
    gameResource.itemSprite = IMG_LoadTexture(renderer, "assets/Sprites/items.png");
    
    gameResource.coinSound = ME_LoadAudio("data/audio/coin_collect.wav");
    gameResource.shootSound = ME_LoadAudio("data/audio/shoot.wav");
    gameResource.jumpSound = ME_LoadAudio("data/audio/jump.wav");
    gameResource.hurtSound = ME_LoadAudio("data/audio/hurt.wav");
    
    gameResource.loaded = true;
    
    return gameResource;
}

void LoadEntity(SDL_Renderer *renderer, const char *fileName)
{
    FILE *entityMapFile = fopen(fileName, "r");
    
    if (entityMapFile != NULL)
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
                player = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_PLAYER);
                if (player < MAX_ENTITY_COUNT)
                {
                    //transform
                    ecsWorld.transforms[player] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
                    
                    //render
                    ecsWorld.renders[player].texture = gameResource.playerSprite;
                    ecsWorld.renders[player].width = 64;
                    ecsWorld.renders[player].height = 64;
                    
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
                    ecsWorld.animations[player].animations[Idle] = idleAnim;
                    ecsWorld.animations[player].animations[Walking] = walkAnim;
                    ecsWorld.animations[player].animations[Jump] = jumpAnim;
                    ecsWorld.animations[player].animations[Attack] = shootAnim;
                    ecsWorld.animations[player].width = 16;
                    ecsWorld.animations[player].height = 16;
                    ecsWorld.animations[player].currentAnimationIndex = 0;
                    
                    //stat
                    ecsWorld.stat[player].PlayerStat.health = 100;
                    ecsWorld.stat[player].PlayerStat.facingDir = Right;
                    
                    //physics
                    ecsWorld.physics[player].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 1.0, 45, 64);
                    ecsWorld.physics[player].physicsBody.velocity = Vector2Init(0.0f, 0.0f);
                    ecsWorld.physics[player].physicsBody.affectedByGravity = true;
                    ecsWorld.physics[player].physicsBody.restitution = 0.0f;
                    ecsWorld.physics[player].physicsBody.friction = 0.05f;
                    ecsWorld.physics[player].excludeEntityTag = ENTITY_TAG_NONE;
                    
                    ecsWorld.entitySignature[player] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                break;
                
                case ENTITY_TAG_LIZARD:
                Entity lizard = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_LIZARD);
                if (lizard < MAX_ENTITY_COUNT)
                {
                    //transform
                    ecsWorld.transforms[lizard]= CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
                    
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
                    ecsWorld.animations[lizard].animations[Idle] = lizardIdleAnim;
                    ecsWorld.animations[lizard].animations[Walking] = lizardWalkAnim;
                    ecsWorld.animations[lizard].currentAnimationIndex = 1,
                    ecsWorld.animations[lizard].width = 16,
                    ecsWorld.animations[lizard].height = 16,
                    
                    //render
                    ecsWorld.renders[lizard].texture = gameResource.enemySprite;
                    ecsWorld.renders[lizard].width = 64;
                    ecsWorld.renders[lizard].height = 64;
                    
                    //stat
                    ecsWorld.stat[lizard].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
                    ecsWorld.stat[lizard].EnemyStat.moveRight = !!(lizard % 2); //odd ones moves right
                    ecsWorld.stat[lizard].EnemyStat.patrolDistance = 40.0f;
                    ecsWorld.stat[lizard].EnemyStat.moveRight = false;
                    ecsWorld.stat[lizard].EnemyStat.health = 1.0f;
                    
                    //physics
                    ecsWorld.physics[lizard].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.5f, 64, 64);
                    ecsWorld.physics[lizard].physicsBody.restitution = 0.0f;
                    ecsWorld.physics[lizard].physicsBody.friction = 0.05f;
                    ecsWorld.physics[lizard].excludeEntityTag = ENTITY_TAG_NONE;
                    
                    ecsWorld.entitySignature[lizard] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                break;
                
                case ENTITY_TAG_SLIME:
                Entity slime = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_SLIME);
                if (slime < MAX_ENTITY_COUNT)
                {
                    //transform
                    ecsWorld.transforms[slime] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
                    
                    //render
                    ecsWorld.renders[slime].texture = gameResource.enemySprite;
                    ecsWorld.renders[slime].width = 50;
                    ecsWorld.renders[slime].height = 50;
                    
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
                    ecsWorld.animations[slime].animations[Idle] = slimeIdleAnim;
                    ecsWorld.animations[slime].animations[Walking] = slimeWalkAnim;
                    ecsWorld.animations[slime].width = 16;
                    ecsWorld.animations[slime].height = 16;
                    ecsWorld.animations[slime].currentAnimationIndex = 1;
                    
                    //stat
                    ecsWorld.stat[slime].EnemyStat.health = 1.0f;
                    ecsWorld.stat[slime].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
                    ecsWorld.stat[slime].EnemyStat.moveRight = !!(n % 2); //odd ones moves right
                    ecsWorld.stat[slime].EnemyStat.patrolDistance = 50.0f;
                    
                    //physics
                    PhysicsComponent physics = {0};
                    ecsWorld.physics[slime].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.5, 45, 50);
                    ecsWorld.physics[slime].physicsBody.restitution = 0.0f;
                    ecsWorld.physics[slime].physicsBody.friction = 0.05f;
                    ecsWorld.physics[slime].excludeEntityTag = ENTITY_TAG_NONE;
                    
                    ecsWorld.entitySignature[slime] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                break;
                
                case ENTITY_TAG_FLYEE:
                Entity flyee = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_FLYEE);
                if (flyee < MAX_ENTITY_COUNT)
                {
                    //transform
                    ecsWorld.transforms[flyee] = CreateTransformComponent(Vector2Init((f32)x, (f32)y), Vector2Null(), 0.0f);
                    
                    //render
                    ecsWorld.renders[flyee].texture = gameResource.enemySprite;
                    ecsWorld.renders[flyee].width = 50;
                    ecsWorld.renders[flyee].height = 50;
                    
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
                    ecsWorld.animations[flyee].animations[Idle] = IdleAnim;
                    ecsWorld.animations[flyee].animations[Walking] = WalkAnim;
                    ecsWorld.animations[flyee].width = 16;
                    ecsWorld.animations[flyee].height = 16;
                    ecsWorld.animations[flyee].currentAnimationIndex = 1;
                    
                    //stat
                    ecsWorld.stat[flyee].EnemyStat.health = 1.0f;
                    ecsWorld.stat[flyee].EnemyStat.startPosition = Vector2Init((f32)x, (f32)y);
                    ecsWorld.stat[flyee].EnemyStat.moveRight = !!(n % 2); //odd ones moves right
                    ecsWorld.stat[flyee].EnemyStat.patrolDistance = 50.0f;
                    
                    //physics
                    PhysicsComponent physics = {0};
                    ecsWorld.physics[flyee].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 0.0f, 45, 30);
                    ecsWorld.physics[flyee].physicsBody.restitution = 0.0f;
                    ecsWorld.physics[flyee].physicsBody.friction = 0.05f;
                    ecsWorld.physics[flyee].excludeEntityTag = ENTITY_TAG_NONE;
                    ecsWorld.physics[flyee].physicsBody.affectedByGravity = false;
                    
                    ecsWorld.entitySignature[flyee] = TRANSFORM_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        ENTITYSTAT_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                break;
                
                case ENTITY_TAG_COIN:
                Entity coin  = MECS_CreateEntity(&ecsWorld, ENTITY_TAG_COIN);
                if (coin < MAX_ENTITY_COUNT)
                {
                    //transform
                    ecsWorld.transforms[coin]= CreateTransformComponent(Vector2Init((f32)x, (f32)y), 
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
                    ecsWorld.animations[coin].animations[Idle] = coinAnim;
                    ecsWorld.animations[coin].width = 16;
                    ecsWorld.animations[coin].height = 16;
                    ecsWorld.animations[coin].currentAnimationIndex = Idle;
                    
                    //render
                    ecsWorld.renders[coin].texture = gameResource.itemSprite;
                    ecsWorld.renders[coin].width = 40;
                    ecsWorld.renders[coin].height = 40;
                    
                    //physics
                    ecsWorld.physics[coin].physicsBody = CreatePhysicsBody(Vector2Init((f32)x, (f32)y), 1.0, 32, 32);
                    ecsWorld.physics[coin].physicsBody.restitution = 0.0f;
                    ecsWorld.physics[coin].physicsBody.friction = 0.07f;
                    ecsWorld.physics[coin].excludeEntityTag = ENTITY_TAG_LIZARD | ENTITY_TAG_SLIME | ENTITY_TAG_BULLET;
                    ecsWorld.physics[coin].physicsBody.affectedByGravity = false;
                    
                    ecsWorld.entitySignature[coin] = TRANSFORM_COMPONENT_SIGN |
                        ANIMATION_COMPONENT_SIGN |
                        RENDER_COMPONENT_SIGN |
                        PHYSICS_COMPONENT_SIGN;
                }
                else
                {
                    printf("\nECS world full!!!\n");
                }
                
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

void LoadData(SDL_Renderer *renderer)
{
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    if (!gameResource.loaded)
    {
        gameResource = LoadGameResource(renderer);
        //ME_Audio a = ME_LoadAudio("data/audio/tsf.wav");
        //ME_PlayAudio(&a);
    }
    
    //NOTE(abhicv): resetting things in the last level
    SDL_DestroyTexture(levels[currentLevelIndex].tileMapTexture);
    levels[currentLevelIndex].tileMapTexture = NULL;
    
    memset(&ecsWorld, 0, sizeof(MicroECSWorld));
    score = 0;
    coinCount = 0;
    
    currentLevelIndex = nextLevelIndex;
    LoadEntity(renderer, levels[currentLevelIndex].entityMapFile);
    levels[currentLevelIndex].tileMapTexture = LoadTileMapTexture(renderer, levels[currentLevelIndex].tileMapFile);
    
    //NOTE(abhicv): platform collision entities
    {
        TransformComponent transform = {0};
        RenderComponent render = {0};
        PhysicsComponent physics = {0};
        
        FILE *colMapFile = fopen(levels[currentLevelIndex].collisionMapFile, "r");
        if (colMapFile != NULL)
        {
            u32 platformCount = 0;
            fscanf(colMapFile, "n:%d\n", &platformCount);
            
            for(u32 n = 0; n < platformCount; n++)
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
                    
                    transform.position = Vector2Init(x + (w / 2), y + (h / 2));
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
    
    u32 levelCount = sizeof(levels) / sizeof(Level);
    nextLevelIndex = (nextLevelIndex <  (levelCount - 1)) ? nextLevelIndex + 1 : 0;
}
