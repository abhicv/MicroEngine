#include "MicroEngine.c"

const u32 displayWidth = 1280;
const u32 displayHeight = 720;

typedef struct 
{
    SDL_Texture *displayTexture;
    SDL_Rect displayRect;
    SDL_Rect tileSheetCursor;
    SDL_Rect srcRect;

} TileSheetDisplaySpace;

global TileMap tileMap = {0};
global SDL_Rect tileMapCursor = {0};
global SDL_Rect *activeCursor = NULL;
global char tileMapFileName[15] = "test.tmap";
global TileSheetDisplaySpace tileSheetDisplay = {0};

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

    SDL_Color color = {80, 80, 80, 255};

    //drawing tile grids
    for(j = 0; j < tileMap.tileMapHeight; j++)
    {
        for(i = 0; i < tileMap.tileMapWidth; i++)
        {
            SDL_Rect rect;

            rect.x = i * tileMap.tileWidth;
            rect.y = j * tileMap.tileHeight;
            rect.w = tileMap.tileWidth;
            rect.h = tileMap.tileHeight;

            ME_RenderDrawRect(renderer, &rect, color);
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

void WriteTileMapDataToFile(TileMap *tileMap, const char *fileName)
{
    FILE *tileMapFile = fopen(fileName, "w");

    fprintf(tileMapFile, "//Tile Map description file created by lonecoder\n");
    fprintf(tileMapFile, "w:%d\n", tileMap->tileMapWidth);
    fprintf(tileMapFile, "h:%d\n", tileMap->tileMapHeight);
    fprintf(tileMapFile, "tw:%d\n", tileMap->tileWidth);
    fprintf(tileMapFile, "th:%d\n", tileMap->tileHeight);
    fprintf(tileMapFile, "sw:%d\n", tileMap->tileSheetTileWidth);
    fprintf(tileMapFile, "sh:%d\n", tileMap->tileSheetTileHeight);

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

void HandleEvent(SDL_Event event)
{
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
            break;

        case SDL_SCANCODE_S:
            break;

        case SDL_SCANCODE_A:
            break;

        case SDL_SCANCODE_D:
            break;

        case SDL_SCANCODE_TAB:
            activeCursor = &tileSheetDisplay.tileSheetCursor;
            break;

        case SDL_SCANCODE_SPACE:
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
        }

        break;

    case SDL_KEYUP:
        switch (event.key.keysym.scancode)
        {        
        case SDL_SCANCODE_TAB:
            activeCursor = &tileMapCursor;
            break;
        }
        break;
    }
}

void Update(float deltaTime)
{

}

void Render(SDL_Renderer *renderer)
{
    ME_RenderTileMap(&tileMap, renderer);
    RenderEditorSpace(renderer);
    RenderTileSheetDisplay(renderer); 
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        tileMap = ME_LoadTileMap(argv[1]);
    }
    else
    {
        QueryTileMapInfoFromCli();
    }

    // tileMap = ME_CreateTileMap(32, 20, 32, 32, 16, 16);

    tileMapCursor.x = 0;
    tileMapCursor.y = 0;
    tileMapCursor.w = tileMap.tileWidth;
    tileMapCursor.h = tileMap.tileHeight;
    activeCursor = &tileMapCursor;

    //Intialization
    ME_Game game = ME_CreateGame("MicroEditor", displayWidth, displayHeight);

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

    game.handleEvent = HandleEvent;
    game.update = Update;
    game.render = Render;

    ME_RunGame(&game, false);
    
    WriteTileMapDataToFile(&tileMap, tileMapFileName);

    ME_QuitGame(&game);

    return 0;
}