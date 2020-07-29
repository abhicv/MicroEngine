#include "..\Engine\src\MicroEngine.c"

const u32 displayWidth = 1366;
const u32 displayHeight = 768;
global ME_Game game;

enum EditorMode
{
    EDITOR_MODE_tile,
    EDITOR_MODE_collisionRect,
    EDITOR_MODE_entity,
    EDITOR_MODE_selectTile,
};

typedef struct CollisionSpace
{
    SDL_Rect collisionRects[50];
    SDL_Rect collisionCursor;
    u32 rectCount;

} CollisionSpace;

typedef struct TileSheetDisplaySpace
{
    SDL_Texture *displayTexture;
    SDL_Rect displayRect;
    SDL_Rect tileSheetCursor;
    SDL_Rect srcRect;

} TileSheetDisplaySpace;

char *EditModeToString(u32 mode)
{
    char *modeString;

    switch (mode)
    {
    case EDITOR_MODE_tile:
        modeString = "Mode : Tile";    
        break;
    
    case EDITOR_MODE_entity:
        modeString = "Mode : Entity";    
        break;

    case EDITOR_MODE_collisionRect:
        modeString = "Mode : Collision";    
        break;

    default:
        modeString = "Not A Mode";
        break;
    }
    return modeString;
}

global TileMap tileMap;
global SDL_Rect tileMapCursor;
global SDL_Rect *activeCursor = NULL;
global char tileMapFileName[25] = "default.tmap";
global TileSheetDisplaySpace tileSheetDisplay;
global CollisionSpace collisionSpace;

#define MUI_ORIGIN_ID 2000
global MUI ui;
global MUI_Input uiInput;

u32 editorMode = EDITOR_MODE_tile;

void QueryTileMapInfoFromCli()
{
    u32 choice = 0;

    printf("\nMicroEditor!\n");

    printf("1] New Tile Map\n");
    printf("2] Load Tile Map\n");
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

            printf("Enter tile map file name:");
            scanf("%s", tileMapFileName);

            tileMap = ME_CreateTileMap(tileMapWidth, 
                                        tileMapHeight, 
                                        tileWidth, 
                                        tileHeight, 
                                        tileSheetTileWidth,
                                        tileSheetTileHeight);
            break;

        case 2:
            printf("Enter file name to load:");
            scanf("%s", tileMapFileName);
            tileMap = ME_LoadTileMap(tileMapFileName);  
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

    //drawing tile grids
    for(j = 0; j <= tileMap.tileMapHeight; j++)
    {
        for(i = 0; i <= tileMap.tileMapWidth; i++)
        {
            i32 x = i * tileMap.tileWidth;
            i32 y = j * tileMap.tileHeight;

            ME_RenderDrawPoint(renderer, x, y, color);
        }
    }

    SDL_Rect srcRect = tileSheetDisplay.tileSheetCursor;
    srcRect.x -= tileSheetDisplay.displayRect.x;
    srcRect.y -= tileSheetDisplay.displayRect.y;
    SDL_RenderCopy(renderer, tileSheetDisplay.displayTexture, &srcRect, &tileMapCursor);
    
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

void WriteTileMapDataToFile(TileMap *tileMap, const char *fileName)
{
    FILE *tileMapFile = fopen(fileName, "w");

    //Header for tile map file
    fprintf(tileMapFile, "//Tile Map description file created by lonecoder\n");
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

void WriteCollisionDataToFile(CollisionSpace *colSpace, const char *fileName)
{
    FILE *colMapFile = fopen(fileName, "w");

    fprintf(colMapFile, "n:%d\n", colSpace->rectCount);

    u32 i = 0;
    for (i = 0; i < colSpace->rectCount; i++)
    {
        i32 x = colSpace->collisionRects[i].x;
        i32 y = colSpace->collisionRects[i].y;
        u32 w = colSpace->collisionRects[i].w;
        u32 h = colSpace->collisionRects[i].h;

        fprintf(colMapFile, "c:%d,%d,%d,%d\n", x, y, w, h);
    }
}

void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);

    switch (event.type)
    {
    case SDL_KEYDOWN:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_UP:
            activeCursor->y -= activeCursor->h;
            break;

        case SDL_SCANCODE_DOWN:
            activeCursor->y += activeCursor->h;
            break;

        case SDL_SCANCODE_LEFT:
            activeCursor->x -= activeCursor->w;
            break;

        case SDL_SCANCODE_RIGHT:
            activeCursor->x += activeCursor->w;
            break;

        case SDL_SCANCODE_W:
            collisionSpace.collisionCursor.h -= 32;
            break;

        case SDL_SCANCODE_S:
            collisionSpace.collisionCursor.h += 32;
            break;

        case SDL_SCANCODE_A:
            collisionSpace.collisionCursor.w -= 32;
            break;

        case SDL_SCANCODE_D:
            collisionSpace.collisionCursor.w += 32;
            break;

        case SDL_SCANCODE_TAB:
            if(editorMode == EDITOR_MODE_tile)
                editorMode = EDITOR_MODE_selectTile;
            break;

        case SDL_SCANCODE_SPACE:

            switch (editorMode)
            {
            case EDITOR_MODE_tile:
                u32 x = (tileMapCursor.x / tileMap.tileWidth);
                u32 y = (tileMapCursor.y / tileMap.tileHeight);

                if(x >= 0 && y >= 0 && x < tileMap.tileMapWidth && y < tileMap.tileMapHeight)
                {
                    u32 index = x + (y * tileMap.tileMapWidth);
                    printf("index : %d\n", index);

                    tileMap.tiles[index].tileMapPosX = tileMapCursor.x;
                    tileMap.tiles[index].tileMapPosY = tileMapCursor.y;

                    u32 sheetX = tileSheetDisplay.tileSheetCursor.x - tileSheetDisplay.displayRect.x;
                    u32 sheetY = tileSheetDisplay.tileSheetCursor.y - tileSheetDisplay.displayRect.y;
                    tileMap.tiles[index].tileSheetPosX = sheetX;
                    tileMap.tiles[index].tileSheetPosY = sheetY;
                }
                break;

            case EDITOR_MODE_collisionRect:
                collisionSpace.collisionRects[collisionSpace.rectCount] = collisionSpace.collisionCursor;
                collisionSpace.rectCount++;
                collisionSpace.collisionCursor.w = tileMapCursor.w;
                collisionSpace.collisionCursor.h = tileMapCursor.h;
                break;
            }
            
            break;
        }

        break;

    case SDL_KEYUP:
        switch (event.key.keysym.scancode)
        {        
        case SDL_SCANCODE_TAB:
            if (editorMode == EDITOR_MODE_selectTile)
                editorMode = EDITOR_MODE_tile;
            break;
        }
        break;
    }
}

void Update(float deltaTime)
{
    MUI_BeginFrame(&ui, &uiInput);

    MUI_Rect rect = {1150, 500, 140, 35};
    MUI_PushColumnLayout(&ui, rect, 5);
    MUI_TextA(&ui, GEN_MUI_ID(), EditModeToString(editorMode), 14);

    if (MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit tile"))
    {
        editorMode = EDITOR_MODE_tile;
    }

    if (MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit Collision"))
    {
        editorMode = EDITOR_MODE_collisionRect;
    }

    if (MUI_ButtonA(&ui, GEN_MUI_ID(), "Edit Entity"))
    {
        editorMode = EDITOR_MODE_entity;
    }

    MUI_PopLayout(&ui);

    if (editorMode == EDITOR_MODE_collisionRect)
    {
        activeCursor = &collisionSpace.collisionCursor;
    }
    else if(editorMode == EDITOR_MODE_tile)
    {
        activeCursor = &tileMapCursor;
    }
    else if(editorMode == EDITOR_MODE_selectTile)
    {
        activeCursor = &tileSheetDisplay.tileSheetCursor;
    }
}

void Render(SDL_Renderer *renderer)
{
    ME_RenderTileMap(&tileMap, renderer);
    RenderEditorSpace(renderer);
    RenderTileSheetDisplay(renderer); 
    RenderCollisionSpace(&collisionSpace, renderer);
    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{

// #if 0
    if(argc > 1)
    {
        tileMap = ME_LoadTileMap(argv[1]);
    }
    else
    {
        QueryTileMapInfoFromCli();
    }
// #endif

    ui.fontFile = AGOESTOESAN_FONT_FILE;

    //tileMap = ME_CreateTileMap(42, 24, 32, 32, 16, 16);

    tileMapCursor.x = 0;
    tileMapCursor.y = 0;
    tileMapCursor.w = tileMap.tileWidth;
    tileMapCursor.h = tileMap.tileHeight;
    activeCursor = &tileMapCursor;

    //Intialization
    game = ME_CreateGame("MicroEditor(Made by loneCoder)", displayWidth, displayHeight);

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

    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    ME_RunGame(&game, false);
    
    WriteTileMapDataToFile(&tileMap, tileMapFileName);
    WriteCollisionDataToFile(&collisionSpace, "default.cmap");

    ME_QuitGame(&game);

    return 0;
}