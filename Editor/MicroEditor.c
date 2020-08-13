#include "..\Engine\src\MicroEngine.c"

const u32 displayWidth = 1500;
const u32 displayHeight = 768;

global ME_Game game;

enum EditorMode
{
    EDITOR_MODE_TILE,
    EDITOR_MODE_COLLISION_RECT,
    EDITOR_MODE_ENTITY,
    EDITOR_MODE_SELECT_TILE,
};

typedef struct EntitySpace
{
	SDL_Point entityPositions[MAX_ENTITY_COUNT];
	SDL_Rect entityCursor;
	u32 entityTag[MAX_ENTITY_COUNT];
	u32 entityCount;
	u32 currentEntityTag;
    
} EntitySpace;

#define MAX_COLLISION_RECTS 100
typedef struct CollisionSpace
{
    SDL_Rect collisionRects[MAX_COLLISION_RECTS];
    SDL_Rect collisionCursor;
    u32 rectCount;
    
} CollisionSpace;

typedef struct TileSheetDisplaySpace
{
    SDL_Texture *displayTexture;
    SDL_Rect displayRect;
    SDL_Rect tileSheetCursor;
    
} TileSheetDisplaySpace;

#define MAX_FILE_NAME_SIZE 50
global char tileMapFileName[MAX_FILE_NAME_SIZE] = "default.tmap";
global char collisionMapFileName[MAX_FILE_NAME_SIZE] = "default.cmap";
global char entityMapFileName[MAX_FILE_NAME_SIZE] = "default.emap";

global TileMap tileMap;
global SDL_Rect tileMapCursor;

global TileSheetDisplaySpace tileSheetDisplay;
global CollisionSpace collisionSpace;
global EntitySpace entitySpace;

#define MUI_ORIGIN_ID 2020
global MUI ui;
global MUI_Input uiInput;

u32 editorMode = EDITOR_MODE_TILE;

char *EditModeToString(u32 mode)
{
    char *modeString;
    
    switch (mode)
    {
		case EDITOR_MODE_TILE:
        modeString = "Mode : Tile";
        break;
        
		case EDITOR_MODE_ENTITY:
        modeString = "Mode : Entity";
        break;
        
		case EDITOR_MODE_COLLISION_RECT:
        modeString = "Mode : Collision";
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
	}
	return tagString;
}

CollisionSpace LoadCollisionMap(const char *fileName);
EntitySpace LoadEntityMap(const char *fileName);

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

void RenderTileSheetDisplay(SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, tileSheetDisplay.displayTexture, NULL, &tileSheetDisplay.displayRect);
    SDL_Color color = {255, 255, 0, 0};
    ME_RenderDrawRect(renderer, &tileSheetDisplay.tileSheetCursor, color);
}

void RenderEditorSpace(SDL_Renderer *renderer)
{
    u32 i = 0;
    u32 j = 0;
    
    SDL_Color color = {255, 255, 255, 255};
    
    //drawing tile map grids
    for(j = 0; j <= tileMap.tileMapHeight; j++)
    {
        for(i = 0; i <= tileMap.tileMapWidth; i++)
        {
            i32 x = i * tileMap.tileWidth;
            i32 y = j * tileMap.tileHeight;
            ME_RenderDrawPoint(renderer, x, y, color);
        }
    }
    
    u32 m = 0;
    u32 n = 0;
    
	//rendering tiles
    for(m = 0; m < (tileMapCursor.h / tileMap.tileHeight); m++)
    {
        for(n = 0; n < (tileMapCursor.w / tileMap.tileWidth); n++)
        {
            u32 x = tileMapCursor.x + (tileMap.tileWidth * n);
            u32 y = tileMapCursor.y + (tileMap.tileHeight * m);
            
            u32 xIndex = (x / tileMap.tileWidth);
            u32 yIndex = (y / tileMap.tileHeight);
            
            if(xIndex >= 0 && yIndex >= 0 && xIndex < tileMap.tileMapWidth && yIndex < tileMap.tileMapHeight)
            {
                SDL_Rect srcRect = tileSheetDisplay.tileSheetCursor;
                srcRect.x -= tileSheetDisplay.displayRect.x;
                srcRect.y -= tileSheetDisplay.displayRect.y;
                
                SDL_Rect destRect;
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
    color.b = 0;
    
    ME_RenderDrawRect(renderer, &tileMapCursor, color);
}

void RenderCollisionSpace(CollisionSpace *colSpace, SDL_Renderer *renderer)
{
    SDL_Color color = {255, 255, 255, 0};
    SDL_Color cursorColor = {255, 0, 255, 0};
    
    u32 i = 0;
    for (i = 0; i < colSpace->rectCount; i++)
    {
        ME_RenderDrawRect(renderer, &colSpace->collisionRects[i], color);
    }
    
    ME_RenderDrawRect(renderer, &colSpace->collisionCursor, cursorColor);
}

void RenderEntitySpace(EntitySpace *entitySpace, SDL_Renderer *renderer)
{
    SDL_Color color = {0, 255, 0, 0};
    SDL_Color cursorColor = {0, 0, 255, 0};
    
    u32 i = 0;
    for (i = 0; i < entitySpace->entityCount; i++)
    {
        SDL_Rect rect = tileMapCursor;
        rect.x = entitySpace->entityPositions[i].x - entitySpace->entityCursor.w / 2;
        rect.y = entitySpace->entityPositions[i].y - entitySpace->entityCursor.h / 2;
        rect.w = 32;
        rect.h = 32;
        
        ME_RenderDrawRect(renderer, &rect, color);
    }
    
    ME_RenderDrawRect(renderer, &entitySpace->entityCursor, cursorColor);
}

void WriteTileMapDataToFile(TileMap *tileMap, const char *fileName)
{
    FILE *tileMapFile = fopen(fileName, "w");
    
    //header for tile map file
    fprintf(tileMapFile, "w:%d\n", tileMap->tileMapWidth);
    fprintf(tileMapFile, "h:%d\n", tileMap->tileMapHeight);
    fprintf(tileMapFile, "tw:%d\n", tileMap->tileWidth);
    fprintf(tileMapFile, "th:%d\n", tileMap->tileHeight);
    fprintf(tileMapFile, "sw:%d\n", tileMap->tileSheetTileWidth);
    fprintf(tileMapFile, "sh:%d\n", tileMap->tileSheetTileHeight);
    
    //writing tile data
    u32 i = 0;
    for (i = 0; i < tileMap->tileCount; i++)
    {
        fprintf(tileMapFile, "t:%d,%d,%d,%d\n",
				tileMap->tiles[i].tileMapPosX,
				tileMap->tiles[i].tileMapPosY,
				tileMap->tiles[i].tileSheetPosX,
				tileMap->tiles[i].tileSheetPosY);
    }
    
    fclose(tileMapFile);
}

CollisionSpace LoadCollisionMap(const char *fileName)
{
    FILE *colMapFile = fopen(fileName, "r");
    
    CollisionSpace colSpace = {0};
    u32 count = 0;
    
    if(colMapFile != NULL)
    {
        fscanf(colMapFile, "n:%d\n", &count);
        
        u32 i = 0;
        for(i = 0; i < count; i++)
        {
            fscanf(colMapFile, "c:%d,%d,%d,%d\n",
				   &colSpace.collisionRects[i].x,
				   &colSpace.collisionRects[i].y,
				   &colSpace.collisionRects[i].w,
				   &colSpace.collisionRects[i].h);
        }
        colSpace.rectCount = count;
        fclose(colMapFile);
    }
    
    return colSpace;
}

void WriteCollisionDataToFile(CollisionSpace *colSpace, const char *fileName)
{
    if(colSpace->rectCount > 0)
    {
        FILE *colMapFile = fopen(fileName, "w");
        fprintf(colMapFile, "n:%d\n", colSpace->rectCount);
        
        u32 i = 0;
        for(i = 0; i < colSpace->rectCount; i++)
        {
            fprintf(colMapFile, "c:%d,%d,%d,%d\n",
					colSpace->collisionRects[i].x,
					colSpace->collisionRects[i].y,
					colSpace->collisionRects[i].w,
					colSpace->collisionRects[i].h);
        }
        fclose(colMapFile);
    }
}

EntitySpace LoadEntityMap(const char *fileName)
{
	EntitySpace eSpace = {0};
	FILE *entityFile = fopen(fileName, "r");
    
	if(entityFile != NULL)
	{
		u32 entityCount = 0;
		fscanf(entityFile, "n:%d\n", &entityCount);
        
		u32 i = 0;
		for(i = 0; i < entityCount; i++)
		{
			fscanf(entityFile, "e:%d,%d,%d\n",
				   &eSpace.entityTag[i],
				   &eSpace.entityPositions[i].x,
				   &eSpace.entityPositions[i].y);
		}
        
		eSpace.entityCount = entityCount;
		fclose(entityFile);
	}
    
	return eSpace;
}

void WriteEntityDataToFile(EntitySpace *entitySpace, const char *fileName)
{
	if(entitySpace->entityCount > 0)
	{
		FILE *entityFile = fopen(fileName, "w");
		fprintf(entityFile, "n:%d\n", entitySpace->entityCount);
        
		u32 i = 0;
		for(i = 0; i < entitySpace->entityCount; i++)
		{
			fprintf(entityFile, "e:%d,%d,%d\n",
					entitySpace->entityTag[i],
					entitySpace->entityPositions[i].x,
					entitySpace->entityPositions[i].y);
		}
        
		fclose(entityFile);
	}
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

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);
    
    switch(editorMode)
    {
		case EDITOR_MODE_TILE:
        CursorMovementControl(&tileMapCursor, tileMap.tileWidth, &event);
        CursorSizeControl(&tileMapCursor, tileMap.tileWidth, &event);
        break;
        
		case EDITOR_MODE_COLLISION_RECT:
        CursorMovementControl(&collisionSpace.collisionCursor, tileMap.tileWidth, &event);
        CursorSizeControl(&collisionSpace.collisionCursor, tileMap.tileWidth, &event);
        break;
        
		case EDITOR_MODE_ENTITY:
		CursorMovementControl(&entitySpace.entityCursor, 16, &event);
		break;
        
		case EDITOR_MODE_SELECT_TILE:
        CursorMovementControl(&tileSheetDisplay.tileSheetCursor,tileSheetDisplay.tileSheetCursor.w, &event);
        break;
    }
    
    switch (event.type)
    {
		case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
			case SDLK_TAB:
            if(editorMode == EDITOR_MODE_TILE)
            {
                editorMode = EDITOR_MODE_SELECT_TILE;
            }
            
			if(editorMode == EDITOR_MODE_ENTITY)
			{
				if(entitySpace.currentEntityTag < (1 << 2))
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
                u32 m = 0;
				u32 n = 0;
                
                for(m = 0; m < (tileMapCursor.h / tileMap.tileHeight); m++)
                {
                    for(n = 0; n < (tileMapCursor.w / tileMap.tileWidth); n++)
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
            }
            break;
        }
        break;
        
		case SDL_KEYUP:
        switch (event.key.keysym.sym)
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

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    //entity tag text
	u32 i = 0;
	for(i = 0; i < entitySpace.entityCount; i++)
	{
		MUI_Rect r;
		r.x = entitySpace.entityPositions[i].x;
		r.y = entitySpace.entityPositions[i].y;
		r.width = 120;
		r.height = 30;
        
		MUI_Text(&ui, GEN_MUI_IDi(i), r, EntityTagToString(entitySpace.entityTag[i]), 10);
	}
    
    MUI_Rect rect = {1250, 500, 140, 35};
    rect.x = displayWidth - 100;
    MUI_PushColumnLayout(&ui, rect, 5);
    {
		MUI_TextA(&ui, GEN_MUI_ID(), EntityTagToString(entitySpace.currentEntityTag), 14);
        MUI_TextA(&ui, GEN_MUI_ID(), EditModeToString(editorMode), 14);
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "EDIT tilemap"))
        {
            editorMode = EDITOR_MODE_TILE;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "EDIT collision"))
        {
            editorMode = EDITOR_MODE_COLLISION_RECT;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "EDIT entity"))
        {
            editorMode = EDITOR_MODE_ENTITY;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "Write to File"))
        {
            WriteTileMapDataToFile(&tileMap, tileMapFileName);
            WriteCollisionDataToFile(&collisionSpace, collisionMapFileName);
			WriteEntityDataToFile(&entitySpace, entityMapFileName);
			printf("Data written to files %s, %s, and %s\n", tileMapFileName, collisionMapFileName, entityMapFileName);
        }
    }
    
    MUI_PopLayout(&ui);
}

void Render(SDL_Renderer *renderer)
{
    ME_RenderTileMap(&tileMap, renderer);
    RenderTileSheetDisplay(renderer);
    RenderCollisionSpace(&collisionSpace, renderer);
    RenderEditorSpace(renderer);
	RenderEntitySpace(&entitySpace, renderer);
    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{
	QueryLevelDataFromCli();
    
    ui.fontFile = AGOESTOESAN_FONT_FILE;
    
    // tileMap = ME_CreateTileMap(42, 24, 32, 32, 16, 16);
    
    tileMapCursor.x = 0;
    tileMapCursor.y = 0;
    tileMapCursor.w = tileMap.tileWidth;
    tileMapCursor.h = tileMap.tileHeight;
    
    game = ME_CreateGame("MicroEditor:)", displayWidth, displayHeight);
    
    tileMap.tileSheetTexture = IMG_LoadTexture(game.platform.renderer, "assets/Sprites/tiles.png");
    tileSheetDisplay.displayTexture = tileMap.tileSheetTexture;
    
    SDL_Rect rect = {0};
    SDL_QueryTexture(tileMap.tileSheetTexture, NULL, NULL, &rect.w, &rect.h);
    tileSheetDisplay.displayRect = rect;
    tileSheetDisplay.displayRect.x = displayWidth - tileSheetDisplay.displayRect.w;
    tileSheetDisplay.tileSheetCursor.x = tileSheetDisplay.displayRect.x;
    tileSheetDisplay.tileSheetCursor.y = tileSheetDisplay.displayRect.y;
    tileSheetDisplay.tileSheetCursor.w = tileMap.tileSheetTileWidth;
    tileSheetDisplay.tileSheetCursor.h = tileMap.tileSheetTileHeight;
    
    collisionSpace.collisionCursor = tileMapCursor;
    
	entitySpace.entityCursor = tileMapCursor;
	entitySpace.currentEntityTag = ENTITY_TAG_PLAYER;
    
    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;
    
    ME_RunGame(&game, false);
    
    ME_QuitGame(&game);
    
    return 0;
}