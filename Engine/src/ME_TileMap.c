#include "ME_TileMap.h"

TileMap ME_CreateTileMap(u32 tileMapWidth,
                         u32 tileMapHeight,
                         u32 tileWidth,
                         u32 tileHeight,
                         u32 tileSheetTileWidth,
                         u32 tileSheetTileHeight)
{
    TileMap tileMap = {0};
    u32 tileCount = tileMapWidth * tileMapHeight;
    
    tileMap.tiles = (Tile*)malloc(sizeof(Tile) * tileCount);
    
    u32 i = 0;
    for(i = 0; i < tileCount; i++)
    {
        tileMap.tiles[i].tileMapPosX = -tileWidth;
        tileMap.tiles[i].tileMapPosY = 0;
        tileMap.tiles[i].tileSheetPosX = -tileSheetTileWidth;
        tileMap.tiles[i].tileSheetPosY = 0;
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
    
    if(tileMapFile != NULL)
    {
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
                // printf("w:%d\n", tileMapWidth);
                break;
                
                case 'h':
                fscanf(tileMapFile, ":%d\n", &tileMapHeight);
                // printf("h:%d\n", tileMapHeight);
                break;
                
                case 't':
                a = fgetc(tileMapFile);
                if (a == 'w')
                {
                    fscanf(tileMapFile, ":%d\n", &tileWidth);
                    // printf("tw:%d\n", tileWidth);
                }
                else if (a == 'h')
                {
                    fscanf(tileMapFile, ":%d\n", &tileHeight);
                    // printf("th:%d\n", tileHeight);
                }
                else if (a == ':') //tile coords
                {
                    i32 tileMapPosX = 0;
                    i32 tileMapPosY = 0;
                    i32 tileSheetPosX = 0;
                    i32 tileSheetPosY = 0;
                    
                    fscanf(tileMapFile, "%d,%d,%d,%d\n",
                           &tileMapPosX,
                           &tileMapPosY,
                           &tileSheetPosX,
                           &tileSheetPosY);
                    
                    tileMap.tiles[i].tileMapPosX = tileMapPosX;
                    tileMap.tiles[i].tileMapPosY = tileMapPosY;
                    tileMap.tiles[i].tileSheetPosX = tileSheetPosX;
                    tileMap.tiles[i].tileSheetPosY = tileSheetPosY;
                    
                    i++;
                }
                break;
                
                case 's':
                b = fgetc(tileMapFile);
                if (b == 'w')
                {
                    fscanf(tileMapFile, ":%d\n", &tileSheetTileWidth);
                    // printf("sw:%d\n", tileSheetTileWidth);
                }
                else if (b == 'h')
                {
                    fscanf(tileMapFile, ":%d\n", &tileSheetTileHeight);
                    // printf("sh:%d\n", tileSheetTileHeight);
                    
                    tileMap = ME_CreateTileMap(tileMapWidth,
                                               tileMapHeight,
                                               tileWidth,
                                               tileHeight,
                                               tileSheetTileWidth,
                                               tileSheetTileHeight);
                }
                break;
            }
        }
    }
    
    fclose(tileMapFile);
    return tileMap;
}

void ME_RenderTileMap(TileMap *tileMap, SDL_Renderer *renderer)
{
    u32 i = 0;
    for (i = 0; i < tileMap->tileCount; i++)
    {
        SDL_Rect destRect = {0};
        destRect.x = tileMap->tiles[i].tileMapPosX;
        destRect.y = tileMap->tiles[i].tileMapPosY;
        destRect.w = tileMap->tileWidth;
        destRect.h = tileMap->tileHeight;
        
        SDL_Rect srcRect = {0};
        srcRect.x = tileMap->tiles[i].tileSheetPosX;
        srcRect.y = tileMap->tiles[i].tileSheetPosY;
        srcRect.w = tileMap->tileSheetTileWidth;
        srcRect.h = tileMap->tileSheetTileHeight;
        
        if(tileMap->tileSheetTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMap->tileSheetTexture, &srcRect, &destRect);
        }
    }
}