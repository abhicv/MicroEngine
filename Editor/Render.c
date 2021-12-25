#include "Render.h"

#define HOLDER_RECT_WIDTH 10
void RenderCollisionSpace(CollisionSpace *colSpace, SDL_Renderer *renderer)
{
    SDL_Color color = {255, 255, 0, 255};
    SDL_Color cursorColor = {255, 0, 255, 255};
    
    //collision boxes
    for(u32 i = 0; i < colSpace->rectCount; i++)
    {
        ME_RenderDrawRect(renderer, &colSpace->collisionRects[i], color);
    }
    
    // cursor
    ME_RenderDrawRect(renderer, &colSpace->collisionCursor, cursorColor);
    
    // drawing resize holders for collision rect
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
}

void RenderEntitySpace(EntitySpace *entitySpace, SDL_Renderer *renderer)
{
    SDL_Color color = {0, 255, 0, 0};
    SDL_Color cursorColor = {0, 0, 255, 0};
    
    for(u32 i = 0; i < entitySpace->entityCount; i++)
    {
        SDL_Rect rect = {0};
        rect.x = entitySpace->entityPositions[i].x - entitySpace->entityCursor.w / 2;
        rect.y = entitySpace->entityPositions[i].y - entitySpace->entityCursor.h / 2;
        rect.w = entitySpace->entityCursor.w;
        rect.h = entitySpace->entityCursor.h;
        
        ME_RenderDrawRect(renderer, &rect, color);
    }
    
    ME_RenderDrawRect(renderer, &entitySpace->entityCursor, cursorColor);
}

void RenderTileSheetDisplay(TileSheetDisplaySpace *tileSheetDisplaySpace, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, tileSheetDisplaySpace->displayTexture, 0, &tileSheetDisplaySpace->displayRect);
    SDL_Color color = {255, 255, 0, 0};
    ME_RenderDrawRect(renderer, &tileSheetDisplaySpace->tileSheetCursor, color);
}
