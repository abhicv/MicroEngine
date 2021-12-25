#include "MicroEditor.h"
#include "File.c"
#include "Render.c"
#include "..\Engine\src\MicroEngine.c"

const u32 displayWidth = 1600;
const u32 displayHeight = 900;

global SDL_Window *window;
global SDL_Renderer *renderer;
global SDL_Event event;

#define MAX_FILE_NAME_SIZE 100
global u8 tileMapFileName[MAX_FILE_NAME_SIZE] = "tmp.tmap";
global u8 collisionMapFileName[MAX_FILE_NAME_SIZE] = "tmp.cmap";
global u8 entityMapFileName[MAX_FILE_NAME_SIZE] = "tmp.emap";

global TileMap tileMap;
global TileSheetDisplaySpace tileSheetDisplay;
global CollisionSpace collisionSpace;
global EntitySpace entitySpace;

//cursors
global SDL_Rect deleteCursor;
global SDL_Rect tileMapCursor;

//app mode
global u32 appMode = APP_MODE_MAIN_MENU;

//editor modes
global u32 editorMode = EDITOR_MODE_TILE;
global u32 lastEditorMode = EDITOR_MODE_TILE;

#define MUI_ORIGIN_ID 2021
global MUI ui;
global MUI_Input uiInput;

//UI Style
MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {100, 100, 200},
        .highlightColor = {50, 50, 100},
        .textColor = {255, 255, 255},
        .fontSize  = 22,
    },
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

u32 Power(u32 x, u32 power)
{
    u32 result = 1;
    
    for(u32 n = 0; n < power; n++)
    {
        result *= x;
    }
    
    return result;
}

u32 StringToInteger(u8 *str, u32 size)
{
    u32 num = 0;
    u32 power = size - 1;
    
    for(u32 n = 0; n < size; n++)
    {
        num += (str[n] - '0') * Power(10, power);
        power--;
    }
    
    return num;
}

void InitializeEditorData()
{
    tileMapCursor.x = 0;
    tileMapCursor.y = 0;
    tileMapCursor.w = tileMap.tileWidth;
    tileMapCursor.h = tileMap.tileHeight;
    deleteCursor = tileMapCursor;
    
    tileMap.tileSheetTexture = IMG_LoadTexture(renderer, "assets/Sprites/tiles.png");
    
    SDL_Rect rect = {0};
    SDL_QueryTexture(tileMap.tileSheetTexture, NULL, NULL, &rect.w, &rect.h);
    
    tileSheetDisplay.displayRect.w = rect.w;
    tileSheetDisplay.displayRect.h = rect.h;
    tileSheetDisplay.displayRect.x = displayWidth - tileSheetDisplay.displayRect.w;
    tileSheetDisplay.displayRect.y = 0;
    
    tileSheetDisplay.tileSheetCursor.x = tileSheetDisplay.displayRect.x;
    tileSheetDisplay.tileSheetCursor.y = tileSheetDisplay.displayRect.y;
    tileSheetDisplay.tileSheetCursor.w = tileMap.tileSheetTileWidth;
    tileSheetDisplay.tileSheetCursor.h = tileMap.tileSheetTileHeight;
    
    tileSheetDisplay.displayTexture = tileMap.tileSheetTexture;
    
    collisionSpace.collisionCursor = tileMapCursor;
    collisionSpace.highlightedColRectIndex = MAX_COLLISION_RECTS;
    
    entitySpace.entityCursor = tileMapCursor;
    entitySpace.currentEntityTag = ENTITY_TAG_PLAYER;
}

char *EditModeToString(u32 mode)
{
    char *modeString = NULL;
    
    switch (mode)
    {
		case EDITOR_MODE_TILE:
        modeString = "Mode : Tile\0";
        break;
        
		case EDITOR_MODE_ENTITY:
        modeString = "Mode : Entity\0";
        break;
        
		case EDITOR_MODE_COLLISION_RECT:
        modeString = "Mode : Collision\0";
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        modeString = "Mode : Select\0";
        break;
        
        case EDITOR_MODE_DELETE:
        modeString = "Mode : Delete\0";
        break;
    }
    
    return modeString;
}

char *EntityTagToString(u32 tag)
{
	char *tagString = "NONE";
    
	switch(tag)
	{
		case ENTITY_TAG_PLAYER:
		tagString = "PLAYER";
		break;
        
		case ENTITY_TAG_LIZARD:
		tagString = "LIZARD";
		break;
        
        case ENTITY_TAG_SLIME:
        tagString = "SLIME";
        break;
        
        case ENTITY_TAG_FLYEE:
        tagString = "FYLYEE";
        break;
        
        case ENTITY_TAG_COIN:
        tagString = "COIN";
        break;
        
        case ENTITY_TAG_CAPTURE_FLAG:
        tagString = "FLAG";
        break;
        
        case ENTITY_TAG_CRATE:
        tagString = "CRATE";
        break;
	}
    
	return tagString;
}

b32 IsMouseOverSDLRect(u32 mouseX, u32 mouseY, SDL_Rect rect)
{
    b32 result = false;
    
    if(mouseX > rect.x && mouseX < (rect.x + rect.w) &&
       mouseY > rect.y && mouseY < (rect.y + rect.h))
    {
        result = true;
    }
    
    return result;
}

b32 QueryLevelDataFromCli(void)
{
    u32 choice = 0;
    
    printf("\nWelcome to MicroEditor!\n\n");
    
    printf("[1] New level\n");
    printf("[2] Load level\n");
    printf("[3] Exit\n");
    printf("Enter your choice:");
    scanf("%d", &choice);
    
    u32 tileMapWidth = 0;
    u32 tileMapHeight = 0;
    u32 tileWidth = 0;
    u32 tileHeight = 0;
    u32 tileSheetTileWidth = 0;
    u32 tileSheetTileHeight = 0;
    
    switch(choice)
    {
        case 1:
		printf("\nWidth of tile map(tile unit):");
		scanf("%d", &tileMapWidth);
        
		printf("Height of tile map(tile unit):");
		scanf("%d", &tileMapHeight);
        
		printf("Tile width in pixels:");
		scanf("%d", &tileWidth);
        
		printf("Tile height in pixels:");
		scanf("%d", &tileHeight);
        
		printf("TileSheet tile width in pixels:");
		scanf("%d", &tileSheetTileWidth);
        
		printf("TileSheet tile width in pixels:");
		scanf("%d", &tileSheetTileHeight);
        
		tileMap = ME_CreateTileMap(tileMapWidth, tileMapHeight, tileWidth, tileHeight,
								   tileSheetTileWidth, tileSheetTileHeight);
		break;
        
        case 2:
		printf("Enter tile map file name to load:");
		scanf("%s", tileMapFileName);
		tileMap = ME_LoadTileMap(tileMapFileName);
        
		printf("Enter collision map file name to load:");
		scanf("%s", collisionMapFileName);
		collisionSpace = LoadCollisionMap(collisionMapFileName);
        
		printf("Enter entity map file name to load:");
		scanf("%s", entityMapFileName);
		entitySpace = LoadEntityMap(entityMapFileName);
		break;
        
        case 3:
        return 1;
        break;
        
        default:
		tileMap = ME_CreateTileMap(32, 20, 32, 32, 16, 16);
		break;
    }
    
    return 0;
}

void RenderEditorSpace(SDL_Renderer *renderer)
{
    SDL_Color color = {80, 80, 80, 255};
    
    //drawing tile map grids
    for(u32 j = 0; j < tileMap.tileMapHeight; j++)
    {
        for(u32 i = 0; i < tileMap.tileMapWidth; i++)
        {
            i32 x = i * tileMap.tileWidth;
            i32 y = j * tileMap.tileHeight;
            SDL_Rect rect = {x, y, tileMap.tileWidth, tileMap.tileHeight};
            ME_RenderDrawRect(renderer, &rect, color);
        }
    }
    
	//rendering tiles
    for(u32 m = 0; m < (tileMapCursor.h / tileMap.tileHeight); m++)
    {
        for(u32 n = 0; n < (tileMapCursor.w / tileMap.tileWidth); n++)
        {
            u32 x = tileMapCursor.x + (tileMap.tileWidth * n);
            u32 y = tileMapCursor.y + (tileMap.tileHeight * m);
            
            u32 xIndex = (x / tileMap.tileWidth);
            u32 yIndex = (y / tileMap.tileHeight);
            
            if(xIndex >= 0 && yIndex >= 0 && xIndex < tileMap.tileMapWidth && yIndex < tileMap.tileMapHeight)
            {
                SDL_Rect srcRect = {0};
                srcRect.x = tileSheetDisplay.tileSheetCursor.x - tileSheetDisplay.displayRect.x;
                srcRect.y = tileSheetDisplay.tileSheetCursor.y - tileSheetDisplay.displayRect.y;
                srcRect.w = tileSheetDisplay.tileSheetCursor.w;
                srcRect.h = tileSheetDisplay.tileSheetCursor.h;
                
                SDL_Rect destRect = {0};
                destRect.x = x;
                destRect.y = y;
                destRect.w = tileMap.tileWidth;
                destRect.h = tileMap.tileHeight;
                
                SDL_RenderCopy(renderer, tileSheetDisplay.displayTexture, &srcRect, &destRect);
            }
        }
    }
    
    color = (SDL_Color){255, 255, 255, 255};
    ME_RenderDrawRect(renderer, &tileMapCursor, color);
    
    color = (SDL_Color){0, 255, 0, 255};
    ME_RenderDrawRect(renderer, &deleteCursor, color);
}

void CursorMovementControl(SDL_Rect *cursor, u32 deltaStep, SDL_Event *event)
{
    switch(event->type)
    {
        case SDL_KEYDOWN:
        switch(event->key.keysym.sym)
        {
            case SDLK_UP:
            cursor->y -= deltaStep;
            break;
            
            case SDLK_DOWN:
            cursor->y += deltaStep;
            break;
            
            case SDLK_LEFT:
            cursor->x -= deltaStep;
            break;
            
            case SDLK_RIGHT:
            cursor->x += deltaStep;
            break;
        }
    }
}

void CursorSizeControl(SDL_Rect *cursor, u32 deltaSize, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_w:
            cursor->h -= deltaSize;
            break;
            
            case SDLK_s:
            cursor->h += deltaSize;
            break;
            
            case SDLK_a:
            cursor->w -= deltaSize;
            break;
            
            case SDLK_d:
            cursor->w += deltaSize;
            break;
        }
    }
}

void EditorHandleEvent(SDL_Event *event)
{
    MUI_GetInput(&uiInput, event);
    
    switch(editorMode)
    {
        case EDITOR_MODE_TILE:
        CursorMovementControl(&tileMapCursor, tileMap.tileWidth, event);
        CursorSizeControl(&tileMapCursor, tileMap.tileWidth, event);
        break;
        
        case EDITOR_MODE_COLLISION_RECT:
        CursorMovementControl(&collisionSpace.collisionCursor, tileMap.tileWidth, event);
        CursorSizeControl(&collisionSpace.collisionCursor, tileMap.tileWidth, event);
        break;
        
        case EDITOR_MODE_ENTITY:
        CursorMovementControl(&entitySpace.entityCursor, tileMap.tileWidth, event);
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        CursorMovementControl(&tileSheetDisplay.tileSheetCursor, tileSheetDisplay.tileSheetCursor.w, event);
        break;
        
        case EDITOR_MODE_DELETE:
        CursorMovementControl(&deleteCursor, tileMap.tileWidth, event);
        CursorSizeControl(&deleteCursor, tileMap.tileWidth, event);
        break;
    }
    
    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_TAB:
            if(editorMode == EDITOR_MODE_TILE)
            {
                editorMode = EDITOR_MODE_SELECT_TILE;
            }
            else if(editorMode == EDITOR_MODE_ENTITY)
            {
                if(entitySpace.currentEntityTag < (1 << 6))
                {
                    entitySpace.currentEntityTag = entitySpace.currentEntityTag << 1;
                }
                else
                {
                    entitySpace.currentEntityTag = 1;
                }
            }
            break;
            
            case SDLK_SPACE:
            switch (editorMode)
            {
                case EDITOR_MODE_TILE:
                for(u32 m = 0; m < (tileMapCursor.h / tileMap.tileHeight); m++)
                {
                    for(u32 n = 0; n < (tileMapCursor.w / tileMap.tileWidth); n++)
                    {
                        u32 x = tileMapCursor.x + (tileMap.tileWidth * n);
                        u32 y = tileMapCursor.y + (tileMap.tileHeight * m);
                        
                        u32 xIndex = (x / tileMap.tileWidth);
                        u32 yIndex = (y / tileMap.tileHeight);
                        
                        if(xIndex >= 0 && yIndex >= 0 && xIndex < tileMap.tileMapWidth && yIndex < tileMap.tileMapHeight)
                        {
                            u32 index = xIndex + (yIndex * tileMap.tileMapWidth);
                            
                            tileMap.tiles[index].tileMapPosX = x;
                            tileMap.tiles[index].tileMapPosY = y;
                            
                            u32 sheetX = tileSheetDisplay.tileSheetCursor.x - tileSheetDisplay.displayRect.x;
                            u32 sheetY = tileSheetDisplay.tileSheetCursor.y - tileSheetDisplay.displayRect.y;
                            tileMap.tiles[index].tileSheetPosX = sheetX;
                            tileMap.tiles[index].tileSheetPosY = sheetY;
                        }
                    }
                }
                break;
                
                case EDITOR_MODE_COLLISION_RECT:
                collisionSpace.collisionRects[collisionSpace.rectCount] = collisionSpace.collisionCursor;
                collisionSpace.rectCount++;
                break;
                
                case EDITOR_MODE_ENTITY:
                entitySpace.entityPositions[entitySpace.entityCount].x = entitySpace.entityCursor.x + entitySpace.entityCursor.w / 2;
                entitySpace.entityPositions[entitySpace.entityCount].y = entitySpace.entityCursor.y + entitySpace.entityCursor.h / 2;
                entitySpace.entityTag[entitySpace.entityCount] = entitySpace.currentEntityTag;
                entitySpace.entityCount++;
                break;
                
                case EDITOR_MODE_DELETE:
                switch(lastEditorMode)
                {
                    case EDITOR_MODE_COLLISION_RECT:
                    for(u32 n = 0; n < collisionSpace.rectCount; n++)
                    {
                        i32 x = collisionSpace.collisionRects[n].x;
                        i32 y = collisionSpace.collisionRects[n].y;
                        
                        if(x >= deleteCursor.x && y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) && y <= (deleteCursor.y + deleteCursor.h))
                        {
                            collisionSpace.collisionRects[n] = collisionSpace.collisionRects[collisionSpace.rectCount - 1];
                            collisionSpace.rectCount -= 1;
                            //break;
                        }
                    }
                    break;
                    
                    case EDITOR_MODE_ENTITY:
                    for(u32 n = 0; n < entitySpace.entityCount; n++)
                    {
                        i32 x = entitySpace.entityPositions[n].x;
                        i32 y = entitySpace.entityPositions[n].y;
                        
                        if(x >= deleteCursor.x && y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) &&
                           y <= (deleteCursor.y + deleteCursor.h))
                        {
                            entitySpace.entityPositions[n] = entitySpace.entityPositions[entitySpace.entityCount - 1];
                            entitySpace.entityTag[n] = entitySpace.entityTag[entitySpace.entityCount - 1];
                            entitySpace.entityCount -= 1;
                            //break;
                        }
                    }
                    break;
                }
                break;
            }
            break;
            
            case SDLK_DELETE:
            lastEditorMode = editorMode;
            editorMode = EDITOR_MODE_DELETE;
            break;
        }
        break;
        
        case SDL_KEYUP:
        switch (event->key.keysym.sym)
        {
            case SDLK_TAB:
            if (editorMode == EDITOR_MODE_SELECT_TILE)
            {
                editorMode = EDITOR_MODE_TILE;
            }
            break;
        }
        break;
    }
}

void EditorUpdateAndRender(SDL_Renderer *renderer)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    //entity tag name text ui
    for(u32 i = 0; i < entitySpace.entityCount; i++)
    {
        MUI_Rect rect = {0};
        rect.width = entitySpace.entityCursor.w;
        rect.height = entitySpace.entityCursor.h;
        rect.x = entitySpace.entityPositions[i].x - rect.width / 2;
        rect.y = entitySpace.entityPositions[i].y - rect.height / 2;
        MUI_Text(&ui, GEN_MUI_IDi(i), rect, EntityTagToString(entitySpace.entityTag[i]), 18, textStyle, MUI_TEXT_ALIGN_MIDDLE);
    }
    
    //checking for collision rects mouse over
    if(editorMode == EDITOR_MODE_COLLISION_RECT)
    {
        collisionSpace.highlightedColRectIndex = MAX_COLLISION_RECTS;
        for(u32 i = 0; i < collisionSpace.rectCount; i++)
        {
            if(IsMouseOverSDLRect(uiInput.mouseX, uiInput.mouseY, collisionSpace.collisionRects[i]))
            {
                collisionSpace.highlightedColRectIndex = i;
                break;
            }
        }
    }
    
    MUI_Rect rect = {displayWidth - 220, 500, 200, 40};
    MUI_PushColumnLayout(&ui, rect, 8);
    {
        MUI_TextA(&ui, GEN_MUI_ID(), EntityTagToString(entitySpace.currentEntityTag), 20, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        MUI_TextA(&ui, GEN_MUI_ID(), EditModeToString(editorMode), 20, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "edit tilemap", buttonStyle))
        {
            editorMode = EDITOR_MODE_TILE;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "edit collision", buttonStyle))
        {
            editorMode = EDITOR_MODE_COLLISION_RECT;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "edit entity", buttonStyle))
        {
            editorMode = EDITOR_MODE_ENTITY;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "save file", buttonStyle))
        {
            WriteTileMapDataToFile(&tileMap, tileMapFileName);
            WriteCollisionDataToFile(&collisionSpace, collisionMapFileName);
            WriteEntityDataToFile(&entitySpace, entityMapFileName);
            printf("Data written to files %s, %s, and %s\n", tileMapFileName, collisionMapFileName, entityMapFileName);
        }
    }
    MUI_PopLayout(&ui);
    
    ME_RenderTileMap(&tileMap, renderer);
    RenderEditorSpace(renderer);
    RenderTileSheetDisplay(&tileSheetDisplay, renderer);
    RenderCollisionSpace(&collisionSpace, renderer);
    RenderEntitySpace(&entitySpace, renderer);
    
    MUI_EndFrame(&ui, renderer);
}

void EditorLoop(SDL_Renderer *renderer)
{
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&event) > 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                appMode = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    appMode = APP_MODE_QUIT;
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    WriteTileMapDataToFile(&tileMap, "tmp.tmap");
                    WriteCollisionDataToFile(&collisionSpace, "tmp.cmap");
                    WriteEntityDataToFile(&entitySpace, "tmp.emap");
                    appMode = APP_MODE_GAME_TEST;
                    quit = true;
                }
                break;
            }
            EditorHandleEvent(&event);
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        EditorUpdateAndRender(renderer);
        SDL_RenderPresent(renderer);
    }
}

void GameTestLoop(SDL_Renderer *renderer)
{
    HMODULE gameCode = 0;
    
    //function pointers for game code
    void (*gameLoadLevelData)(const char *entityMapFile, const char *tileMapFile, const char *collisionMapFile, SDL_Renderer *renderer) = 0;
    void (*gameHandleEvent)(SDL_Event *event) = 0;
    void (*gameUpdateAndRender)(u32 *gameMode, bool *quitGame, f32 deltaTime, SDL_Renderer *renderer) = 0;
    
    gameCode = LoadLibraryA("gamezero.dll");
    if(gameCode)
    {
        gameHandleEvent = GetProcAddress(gameCode, "HandleEvent");
        gameUpdateAndRender = GetProcAddress(gameCode, "GameUpdateAndRender");
        gameLoadLevelData = GetProcAddress(gameCode, "LoadLevelData");
        gameLoadLevelData("tmp.emap", "tmp.tmap", "tmp.cmap", renderer);
    }
    else
    {
        printf("Failed to load game code DLL!!\n");
        appMode = APP_MODE_EDITOR;
        return;
    }
    
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&event) > 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                appMode = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    appMode = APP_MODE_EDITOR;
                }
                break;
            }
            gameHandleEvent(&event);
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        gameUpdateAndRender(0, 0, 0.016f, renderer);
        SDL_RenderPresent(renderer);
    }
    
    FreeLibrary(gameCode);
}

void MainMenuLoop(SDL_Renderer *renderer)
{
    bool quit = false;
    
    const u32 CREAT_LEVEL_MODE = 1;
    const u32 LOAD_LEVEL_MODE = 2;
    const u32 MENU_MODE = 3;
    u32 mode = MENU_MODE;
    
    TextEdit tmapFilePath = {0};
    TextEdit cmapFilePath = {0};
    TextEdit emapFilePath = {0};
    
    TextEdit tileMapWidthText = {0};
    TextEdit tileMapHeightText = {0};
    TextEdit tileWidthText =  {0};
    TextEdit tileHeightText = {0};
    TextEdit tileSheetTileWidthText = {0};
    TextEdit tileSheetTileHeightText = {0};
    
    b32 fileNamesFound = true;
    
    while(!quit)
    {
        uiInput.bTextInput = false;
        uiInput.backSpaceDown = false;
        
        while(SDL_PollEvent(&event) > 0)
        {
            MUI_GetInput(&uiInput, &event);
            
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                appMode = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    appMode = APP_MODE_QUIT;
                }
                break;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        
        MUI_BeginFrame(&ui, &uiInput);
        
        MUI_PushColumnLayout(&ui, (MUI_Rect){10, 10, 230, 35}, 10);
        {
            if(mode == MENU_MODE)
            {
                MUI_TextA(&ui, GEN_MUI_ID(), "MICRO EDITOR- v1.0", 40, textStyle, MUI_TEXT_ALIGN_LEFT);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "create level", buttonStyle))
                {
                    mode = CREAT_LEVEL_MODE;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "load level", buttonStyle))
                {
                    mode = LOAD_LEVEL_MODE;
                }
            }
            
            if(mode == CREAT_LEVEL_MODE)
            {
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Map Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileMapWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Map Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileMapHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Tile Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Tile Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Sheet Tile Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileSheetTileWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Sheet Tile Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileSheetTileHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "back", buttonStyle))
                {
                    mode = MENU_MODE;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "create", buttonStyle))
                {
                    u32 tileMapWidth = StringToInteger(tileMapWidthText.text, strlen(tileMapWidthText.text));
                    u32 tileMapHeight = StringToInteger(tileMapHeightText.text, strlen(tileMapHeightText.text));
                    u32 tileWidth = StringToInteger(tileWidthText.text, strlen(tileWidthText.text));
                    u32 tileHeight = StringToInteger(tileHeightText.text, strlen(tileHeightText.text));
                    u32 tileSheetTileWidth = StringToInteger(tileSheetTileWidthText.text, strlen(tileSheetTileWidthText.text));
                    u32 tileSheetTileHeight = StringToInteger(tileSheetTileHeightText.text, strlen(tileSheetTileHeightText.text));;
                    
                    tileMap = ME_CreateTileMap(tileMapWidth, tileMapHeight, tileWidth, tileHeight, tileSheetTileWidth, tileSheetTileHeight);
                    
                    quit = true;
                    appMode = APP_MODE_EDITOR;
                    
                    InitializeEditorData();
                }
                
                MUI_PopLayout(&ui);
                
            }
            else if(mode == LOAD_LEVEL_MODE)
            {
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "tile map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_Rect r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &tmapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "collision map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &cmapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "entity map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &emapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "back", buttonStyle))
                {
                    mode = MENU_MODE;
                    fileNamesFound = true;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "load", buttonStyle))
                {
                    if(strlen(tmapFilePath.text) > 0)
                    {
                        tileMap = ME_LoadTileMap(tmapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(strlen(cmapFilePath.text) > 0)
                    {
                        collisionSpace = LoadCollisionMap(cmapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(strlen(emapFilePath.text) > 0)
                    {
                        entitySpace = LoadEntityMap(emapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(fileNamesFound)
                    {
                        quit = true;
                        appMode = APP_MODE_EDITOR;
                        InitializeEditorData();
                    }
                }
                MUI_PopLayout(&ui);
                
                Color red = {255, 0, 0 ,255};
                MUI_Style warning;
                warning.textStyle.textColor = red;
                
                if(strlen(tmapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> tile map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }
                
                if(strlen(cmapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> collision map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }
                
                if(strlen(emapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> entity map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }
            }
        }
        MUI_PopLayout(&ui);
        
        MUI_EndFrame(&ui, renderer);
        
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 1;
    }
    
    if(TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize TTF : %s\n", TTF_GetError());
    }
    
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());
    }
    
    window = SDL_CreateWindow("MicroEditor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              displayWidth, displayHeight, 0);
    
    if(window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, 3, SDL_RENDERER_ACCELERATED);
    
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
    
    //ui.fontFile = AGOESTOESAN_FONT_FILE;
    //ui.fontFile = OPENSANS_FONT_FILE;
    ui.fontFile = "assets/Font/OCRAEXT.ttf";
    
    bool quitApp = false;
    while(!quitApp)
    {
        switch(appMode)
        {
            case APP_MODE_EDITOR:
            EditorLoop(renderer);
            break;
            
            case APP_MODE_GAME_TEST:
            GameTestLoop(renderer);
            break;
            
            case APP_MODE_MAIN_MENU:
            MainMenuLoop(renderer);
            break;
            
            case APP_MODE_QUIT:
            quitApp = true;
            break;
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}