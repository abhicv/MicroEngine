#ifndef _RENDER_H
#define _RENDER_H

#include <SDL2/SDL.h>

#include "MicroEditor.h"
#include "..\Engine\src\types.h"
#include "..\Engine\src\ME_Utility.h"

void RenderCollisionSpace(CollisionSpace *colSpace, SDL_Renderer *renderer);
void RenderEntitySpace(EntitySpace *entitySpace, SDL_Renderer *renderer);
void RenderTileSheetDisplay(TileSheetDisplaySpace *tileSheetDisplaySpace, SDL_Renderer *renderer);
#endif //_RENDER_H
