#include "Render.h"

void RenderEditorSpace(SDL_Renderer *renderer, SDL_Rect tileMapCursor, TileMap tileMap, TileSheetDisplaySpace tileSheetDisplay)
{    
    //drawing tile map grids
    // for(u32 j = 0; j < tileMap.tileMapHeight; j++)
    // {
    //     for(u32 i = 0; i < tileMap.tileMapWidth; i++)
    //     {
    //         i32 x = i * tileMap.tileWidth;
    //         i32 y = j * tileMap.tileHeight;
    //         SDL_Rect rect = {x, y, tileMap.tileWidth, tileMap.tileHeight};
    //         ME_RenderDrawRect(renderer, &rect, (SDL_Color){80, 80, 80, 255});
    //         // ME_RenderDrawPoint(renderer, x, y, (SDL_Color){80, 80, 80, 255});
    //     }
    // }

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
                destRect.x = x;
                destRect.y = y;
                destRect.w = tileMap.tileWidth;
                destRect.h = tileMap.tileHeight;
                
                SDL_RenderCopy(renderer, tileSheetDisplay.displayTexture, &srcRect, &destRect);
            }
        }
    }    
}

#define HOLDER_RECT_WIDTH 10
void RenderCollisionSpace(CollisionSpace *colSpace, SDL_Renderer *renderer)
{
    //collision boxes
    for(u32 i = 0; i < colSpace->rectCount; i++)
    {
        ME_RenderDrawRect(renderer, &colSpace->collisionRects[i], (SDL_Color){255, 255, 0, 255});
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

void RenderEntitySpace(EntitySpace *entitySpace, SDL_Renderer *renderer)
{    
    for(u32 i = 0; i < entitySpace->entityCount; i++)
    {
        SDL_Rect rect = {0};
        rect.x = entitySpace->entityPositions[i].x - entitySpace->entityCursor.w / 2;
        rect.y = entitySpace->entityPositions[i].y - entitySpace->entityCursor.h / 2;
        rect.w = entitySpace->entityCursor.w;
        rect.h = entitySpace->entityCursor.h;
        
        ME_RenderDrawRect(renderer, &rect, (SDL_Color){0, 255, 0, 0});
    }    
}

void RenderTileSheetDisplay(TileSheetDisplaySpace *tileSheetDisplaySpace, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, tileSheetDisplaySpace->displayTexture, 0, &tileSheetDisplaySpace->displayRect);
    SDL_Color color = {255, 255, 0, 0};
    ME_RenderDrawRect(renderer, &tileSheetDisplaySpace->tileSheetCursor, color);
}
