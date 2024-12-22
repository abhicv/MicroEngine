#include "ME_TileMap.h"

TileMap ME_CreateTileMap(u32 tileMapWidth, u32 tileMapHeight,
                         u32 tileWidth, u32 tileHeight,
                         u32 tileSheetTileWidth, u32 tileSheetTileHeight)
{
    TileMap tileMap = {0};
    u32 tileCount = tileMapWidth * tileMapHeight;
    
    tileMap.tiles = (Tile*)malloc(sizeof(Tile) * tileCount);
    
    for(int n = 0; n < tileCount; n++)
    {
        tileMap.tiles[n].tileMapPosX = -tileWidth;
        tileMap.tiles[n].tileMapPosY = -tileHeight;
        tileMap.tiles[n].tileSheetPosX = -tileSheetTileWidth;
        tileMap.tiles[n].tileSheetPosY = -tileSheetTileHeight;
    }
    
    tileMap.tileMapWidth = tileMapWidth;
    tileMap.tileMapHeight = tileMapHeight;
    tileMap.tileWidth = tileWidth;
    tileMap.tileHeight = tileHeight;
    tileMap.tileCount = tileCount;
    tileMap.tileSheetTexture = NULL;
    tileMap.tileSheetTileWidth = tileSheetTileWidth;
    tileMap.tileSheetTileHeight = tileSheetTileHeight;
    
    return tileMap;
}

TileMap ME_LoadTileMap(const char *fileName)
{
    TileMap tileMap = {0};
    u32 tileMapWidth = 0;
    u32 tileMapHeight = 0;
    u32 tileWidth = 0;
    u32 tileHeight = 0;
    u32 tileSheetTileWidth = 0;
    u32 tileSheetTileHeight = 0;
    
    FILE *tileMapFile = fopen(fileName, "r");
    
    if (tileMapFile == NULL)
    {
        printf("Failed to load file: '%s'\n", fileName);
        return tileMap;
    }

    char c = 0;
    char a = 0;
    char b = 0;
    
    u32 i = 0;
    while(c != EOF)
    {
        c = fgetc(tileMapFile);
        switch (c)
        {
            case 'w':
            fscanf(tileMapFile, ":%d\n", &tileMapWidth);
            break;
            
            case 'h':
            fscanf(tileMapFile, ":%d\n", &tileMapHeight);
            break;
            
            case 't':
            a = fgetc(tileMapFile);
            if (a == 'w')
            {
                fscanf(tileMapFile, ":%d\n", &tileWidth);
            }
            else if (a == 'h')
            {
                fscanf(tileMapFile, ":%d\n", &tileHeight);
            }
            else if (a == ':') //tile coords
            {                    
                Tile tile = {0};
                fscanf(tileMapFile, "%d,%d,%d,%d\n", &tile.tileMapPosX, &tile.tileMapPosY, &tile.tileSheetPosX, &tile.tileSheetPosY);
                tileMap.tiles[i] = tile;
                i++;
            }
            break;
            
            case 's':
            b = fgetc(tileMapFile);
            if (b == 'w')
            {
                fscanf(tileMapFile, ":%d\n", &tileSheetTileWidth);
            }
            else if (b == 'h')
            {
                fscanf(tileMapFile, ":%d\n", &tileSheetTileHeight);
                
                tileMap = ME_CreateTileMap(tileMapWidth, tileMapHeight,
                                            tileWidth, tileHeight,
                                            tileSheetTileWidth, tileSheetTileHeight);
            }
            break;
        }
    }
    fclose(tileMapFile);
    
    return tileMap;
}

void ME_RenderTileMap(TileMap *tileMap, SDL_Renderer *renderer)
{
    for (u32 n = 0; n < tileMap->tileCount; n++)
    {
        SDL_Rect destRect = {0};
        destRect.x = tileMap->tiles[n].tileMapPosX;
        destRect.y = tileMap->tiles[n].tileMapPosY;
        destRect.w = tileMap->tileWidth;
        destRect.h = tileMap->tileHeight;
        
        SDL_Rect srcRect = {0};
        srcRect.x = tileMap->tiles[n].tileSheetPosX;
        srcRect.y = tileMap->tiles[n].tileSheetPosY;
        srcRect.w = tileMap->tileSheetTileWidth;
        srcRect.h = tileMap->tileSheetTileHeight;
        
        if(tileMap->tileSheetTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMap->tileSheetTexture, &srcRect, &destRect);
        }
    }
}