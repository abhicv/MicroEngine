#include "MicroEditor.h"
#include "File.c"
#include "Render.c"
#include "..\Engine\src\MicroEngine.c"

const u32 displayWidth = 1500;
const u32 displayHeight = 768;

global SDL_Window *window;
global SDL_Renderer *renderer;
global SDL_Event event;

#define MAX_FILE_NAME_SIZE 50
global char tileMapFileName[MAX_FILE_NAME_SIZE] = "default.tmap";
global char collisionMapFileName[MAX_FILE_NAME_SIZE] = "default.cmap";
global char entityMapFileName[MAX_FILE_NAME_SIZE] = "default.emap";

global TileMap tileMap;
global SDL_Rect tileMapCursor;
global TileSheetDisplaySpace tileSheetDisplay;
global CollisionSpace collisionSpace;
global EntitySpace entitySpace;
global SDL_Rect deleteCursor;
global u32 editorMode = EDITOR_MODE_TILE;
global u32 lastEditorMode = EDITOR_MODE_TILE;

#define MUI_ORIGIN_ID 2020
global MUI ui;
global MUI_Input uiInput;

//UI Style
MUI_Style buttonStyle = {
    .buttonStyle = {
        .idleColor = {100, 100, 200},
        .highlightColor = {50, 50, 100},
        .textColor = {255, 255, 255},
        .fontSize  = 13,
    },
};

MUI_Style textStyle = {
    .textStyle = {
        .textColor = {255, 255, 255},
    }
};

char *EditModeToString(u32 mode)
{
    char *modeString = NULL;
    
    switch (mode)
    {
		case EDITOR_MODE_TILE:
        modeString = "Mode : Tile Edit";
        break;
        
		case EDITOR_MODE_ENTITY:
        modeString = "Mode : Entity Edit";
        break;
        
		case EDITOR_MODE_COLLISION_RECT:
        modeString = "Mode : Collision Edit";
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        modeString = "Mode : Select tile";
        break;
        
        case EDITOR_MODE_DELETE:
        modeString = "Mode : Delete";
        break;
    }
    
    return modeString;
}

char *EntityTagToString(u32 tag)
{
	char *tagString = "None";
    
	switch(tag)
	{
		case ENTITY_TAG_PLAYER:
		tagString = "Player";
		break;
        
		case ENTITY_TAG_LIZARD:
		tagString = "Lizard";
		break;
        
        case ENTITY_TAG_SLIME:
        tagString = "Slime";
        break;
        
        case ENTITY_TAG_FLYEE:
        tagString = "flyee";
        break;
        
        case ENTITY_TAG_COIN:
        tagString = "Coin";
        break;
	}
    
	return tagString;
}

void QueryLevelDataFromCli(void)
{
    u32 choice = 0;
    
    printf("\nWelcome to MicroEditor!(created for microengine)\n\n");
    
    printf("[1] New level\n");
    printf("[2] Load level\n");
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
        
		tileMap = ME_CreateTileMap(tileMapWidth,
								   tileMapHeight,
								   tileWidth,
								   tileHeight,
								   tileSheetTileWidth,
								   tileSheetTileHeight);
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
        
        default:
		tileMap = ME_CreateTileMap(32, 20, 32, 32, 16, 16);
		break;
    }
}

void RenderEditorSpace(SDL_Renderer *renderer)
{
    SDL_Color color = {50, 50, 50, 255};
    
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
    
    color.r = 255;
    color.g = 255;
    color.b = 255;
    
    ME_RenderDrawRect(renderer, &tileMapCursor, color);
    
    color.r = 0;
    color.g = 255;
    color.b = 0;
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

void HandleEvent(SDL_Event *event)
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
        CursorMovementControl(&entitySpace.entityCursor, tileMap.tileWidth / 2, event);
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        CursorMovementControl(&tileSheetDisplay.tileSheetCursor,tileSheetDisplay.tileSheetCursor.w, event);
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
            if(editorMode == EDITOR_MODE_ENTITY)
            {
                if(entitySpace.currentEntityTag < (1 << 4))
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
                        
                        if(x >= deleteCursor.x &&
                           y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) &&
                           y <= (deleteCursor.y + deleteCursor.h))
                        {
                            collisionSpace.collisionRects[n] = collisionSpace.collisionRects[collisionSpace.rectCount - 1];
                            collisionSpace.rectCount -= 1;
                            break;
                        }
                    }
                    break;
                    
                    case EDITOR_MODE_ENTITY:
                    for(u32 n = 0; n < entitySpace.entityCount; n++)
                    {
                        i32 x = entitySpace.entityPositions[n].x;
                        i32 y = entitySpace.entityPositions[n].y;
                        
                        if(x >= deleteCursor.x &&
                           y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) &&
                           y <= (deleteCursor.y + deleteCursor.h))
                        {
                            entitySpace.entityPositions[n] = entitySpace.entityPositions[entitySpace.entityCount - 1];
                            entitySpace.entityTag[n] = entitySpace.entityTag[entitySpace.entityCount - 1];
                            
                            entitySpace.entityCount -= 1;
                            break;
                        }
                    }
                    break;
                }
                break;
            }
            break;
            
            case SDLK_e:
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

void UpdateAndRender(float deltaTime, SDL_Renderer *renderer)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    //entity tag ui text
    for(u32 i = 0; i < entitySpace.entityCount; i++)
    {
        MUI_Rect rect = {0};
        rect.x = entitySpace.entityPositions[i].x;
        rect.y = entitySpace.entityPositions[i].y;
        rect.width = 120;
        rect.height = 30;
        
        MUI_Text(&ui, GEN_MUI_IDi(i), rect, EntityTagToString(entitySpace.entityTag[i]), 10, textStyle);
    }
    
    MUI_Rect rect = {1250, 500, 180, 35};
    rect.x = displayWidth - 100;
    MUI_PushColumnLayout(&ui, rect, 8);
    {
        MUI_TextA(&ui, GEN_MUI_ID(), EntityTagToString(entitySpace.currentEntityTag), 14, textStyle);
        MUI_TextA(&ui, GEN_MUI_ID(), EditModeToString(editorMode), 14, textStyle);
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit tilemap", buttonStyle))
        {
            editorMode = EDITOR_MODE_TILE;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit collision", buttonStyle))
        {
            editorMode = EDITOR_MODE_COLLISION_RECT;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit entity", buttonStyle))
        {
            editorMode = EDITOR_MODE_ENTITY;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Write to Files", buttonStyle))
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

int main(int argc, char *argv[])
{
    QueryLevelDataFromCli();
    
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
    
    window = SDL_CreateWindow("MicroEditor",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              displayWidth,
                              displayHeight,
                              0);
    
    if(window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, 3, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
    
    //Setting up data
    {
        ui.fontFile = AGOESTOESAN_FONT_FILE;
        
        // tileMap = ME_CreateTileMap(42, 24, 32, 32, 16, 16);
        
        tileMapCursor.x = 0;
        tileMapCursor.y = 0;
        tileMapCursor.w = tileMap.tileWidth;
        tileMapCursor.h = tileMap.tileHeight;
        deleteCursor = tileMapCursor;
        
        tileMap.tileSheetTexture = IMG_LoadTexture(renderer, "assets/Sprites/tiles.png");
        tileSheetDisplay.displayTexture = tileMap.tileSheetTexture;
        
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
        
        collisionSpace.collisionCursor = tileMapCursor;
        
        entitySpace.entityCursor = tileMapCursor;
        entitySpace.currentEntityTag = ENTITY_TAG_PLAYER;
    }
    
    bool quit = false;
    f32 deltaTime = 0.016f;
    u64 startTime = 0;
    u64 endTime = 0;
    
    while(!quit)
    {
        startTime = SDL_GetPerformanceCounter();
        
        while(SDL_PollEvent(&event) != 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
            }
            HandleEvent(&event);
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        UpdateAndRender(deltaTime, renderer);
        SDL_RenderPresent(renderer);
        
        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;
        
        if(deltaTime > 0.016f)
        {
            deltaTime = 0.016f;
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}