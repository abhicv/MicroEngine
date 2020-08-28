#include "Render.h"

void RenderCollisionSpace(CollisionSpace *colSpace, SDL_Renderer *renderer)
{
    SDL_Color color = {255, 255, 0, 0};
    SDL_Color cursorColor = {255, 0, 255, 0};
    
    for(u32 i = 0; i < colSpace->rectCount; i++)
    {
        ME_RenderDrawRect(renderer, &colSpace->collisionRects[i], color);
    }
    ME_RenderDrawRect(renderer, &colSpace->collisionCursor, cursorColor);
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
        rect.w = 32;
        rect.h = 32;
        
        ME_RenderDrawRect(renderer, &rect, color);
    }
    ME_RenderDrawRect(renderer, &entitySpace->entityCursor, cursorColor);
}

void RenderTileSheetDisplay(TileSheetDisplaySpace *tileSheetDisplaySpace, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, tileSheetDisplaySpace->displayTexture, NULL, &tileSheetDisplaySpace->displayRect);
    SDL_Color color = {255, 255, 0, 0};
    ME_RenderDrawRect(renderer, &tileSheetDisplaySpace->tileSheetCursor, color);
}
