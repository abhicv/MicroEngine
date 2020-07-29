#ifndef ME_TILEMAP_H
#define ME_TILEMAP_H

#include <SDL2/SDL.h>

#include "types.h"
#include "stdlib.h"

typedef struct Tile
{
    i32 tileMapPosX;
    i32 tileMapPosY;
    i32 tileSheetPosX;
    i32 tileSheetPosY;

} Tile;

typedef struct TileMap
{
    Tile *tiles;
    u32 tileMapWidth;
    u32 tileMapHeight;
    u32 tileWidth;
    u32 tileHeight;
    u32 tileCount;

    SDL_Texture *tileSheetTexture;
    u32 tileSheetTileWidth;
    u32 tileSheetTileHeight;

} TileMap;

#endif
