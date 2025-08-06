#include "Render.h"

void RenderTileMap(TileMap tileMap, SDL_Renderer *renderer, f32 zoom, SDL_Point origin)
{
    for (u32 n = 0; n < tileMap.tileCount; n++)
    {
        SDL_Rect destRect = {0};

        Tile tile = tileMap.tiles[n];

        destRect.x = origin.x + (tile.tileMapPosX * zoom);
        destRect.y = origin.y + (tile.tileMapPosY * zoom);
        destRect.w = tileMap.tileWidth * zoom;
        destRect.h = tileMap.tileHeight * zoom;
        
        SDL_Rect srcRect = {0};
        srcRect.x = tile.tileSheetPosX;
        srcRect.y = tile.tileSheetPosY;
        srcRect.w = tileMap.tileSheetTileWidth;
        srcRect.h = tileMap.tileSheetTileHeight;
        
        if(tileMap.tileSheetTexture != NULL)
        {
            SDL_RenderCopy(renderer, tileMap.tileSheetTexture, &srcRect, &destRect);
        }
    }
}

void RenderEditorSpace(SDL_Renderer *renderer, SDL_Rect tileMapCursor, TileMap tileMap, TileSheetDisplaySpace tileSheetDisplay, f32 zoom, SDL_Point origin, bool enableGridLines, bool drawCursorPreview)
{    
    // drawing tile map grids

    if(enableGridLines) {
        for(u32 j = 0; j < tileMap.tileMapHeight; j++)
        {
            for(u32 i = 0; i < tileMap.tileMapWidth; i++)
            {
                i32 x = origin.x + (i * tileMap.tileWidth * zoom);
                i32 y = origin.y + (j * tileMap.tileHeight * zoom);
                SDL_Rect rect = {x, y, tileMap.tileWidth * zoom, tileMap.tileHeight * zoom};
                ME_RenderDrawRect(renderer, &rect, (SDL_Color){80, 80, 80, 255});
            }
        }
    }

    if(drawCursorPreview) {
        // rendering tile map cursor tile preview
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
                    destRect.x = (x * zoom) + origin.x;
                    destRect.y = (y * zoom) + origin.y;
                    destRect.w = tileMap.tileWidth * zoom;
                    destRect.h = tileMap.tileHeight * zoom;
                    
                    SDL_RenderCopy(renderer, tileSheetDisplay.displayTexture, &srcRect, &destRect);
                }
            }
        }
    }

    // drawing tilemap editing space border
    SDL_Rect editorSpaceBorderRect = {
        .x = origin.x,
        .y = origin.y,
        .w = tileMap.tileMapWidth * tileMap.tileWidth * zoom,
        .h = tileMap.tileMapHeight * tileMap.tileHeight * zoom
    };

    ME_RenderDrawRect(renderer, &editorSpaceBorderRect, (SDL_Color){200, 200, 200, 255});

}

#define HOLDER_RECT_WIDTH 10
void RenderCollisionSpace(CollisionSpace colSpace, SDL_Renderer *renderer, f32 zoom, SDL_Point origin, bool enablePreview)
{
    if(enablePreview) {
        for(u32 i = 0; i < colSpace.rectCount; i++)
        {
            SDL_Rect rect = {
                .x = origin.x + (colSpace.collisionRects[i].x * zoom),            
                .y = origin.y + (colSpace.collisionRects[i].y * zoom),            
                .w = colSpace.collisionRects[i].w * zoom,            
                .h = colSpace.collisionRects[i].h * zoom,            
            };
            ME_RenderDrawRect(renderer, &rect, (SDL_Color){255, 255, 0, 255});
        }
    }
    
    // drawing resize holders for collision rect
    /*
    if(colSpace->highlightedColRectIndex < MAX_COLLISION_RECTS)
    {
        SDL_Rect holderRect = colSpace->collisionRects[colSpace->highlightedColRectIndex];
        SDL_Rect hColRect = holderRect;
        
        //top left
        holderRect.w = HOLDER_RECT_WIDTH;
        holderRect.h = HOLDER_RECT_WIDTH;
        ME_RenderFillRect(renderer, &holderRect, color);
        
        //top right
        holderRect.x = hColRect.x + hColRect.w - holderRect.w;
        holderRect.y = holderRect.y;
        ME_RenderFillRect(renderer, &holderRect, color);
        
        //bottom left
        holderRect.x = hColRect.x;
        holderRect.y = hColRect.y + hColRect.h - holderRect.h;
        ME_RenderFillRect(renderer, &holderRect, color);
        
        //bottom right
        holderRect.x = hColRect.x + hColRect.w - holderRect.w;
        holderRect.y = hColRect.y + hColRect.h - holderRect.h;
        ME_RenderFillRect(renderer, &holderRect, color);
    }
    */
}

void RenderEntitySpace(EntitySpace entitySpace, SDL_Renderer *renderer, f32 zoom, SDL_Point origin)
{    
    for(u32 i = 0; i < entitySpace.entityCount; i++)
    {
        SDL_Rect rect = {0};
        rect.x = origin.x + (entitySpace.entityPositions[i].x - entitySpace.entityCursor.w / 2) * zoom;
        rect.y = origin.y + (entitySpace.entityPositions[i].y - entitySpace.entityCursor.h / 2) * zoom;
        rect.w = entitySpace.entityCursor.w * zoom;
        rect.h = entitySpace.entityCursor.h * zoom;
        ME_RenderDrawRect(renderer, &rect, (SDL_Color){0, 255, 0, 0});
    }    
}

void RenderTileSheetDisplay(TileSheetDisplaySpace tileSheetDisplaySpace, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, tileSheetDisplaySpace.displayTexture, 0, &tileSheetDisplaySpace.displayRect);
    SDL_Color color = {255, 255, 0, 0};
    ME_RenderDrawRect(renderer, &tileSheetDisplaySpace.tileSheetCursor, color);
}
