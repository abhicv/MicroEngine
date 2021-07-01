#ifndef _MICRO_EDITOR_H
#define _MICRO_EDITOR_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <Windows.h>

#include "..\Engine\src\types.h"
#include "..\Engine\src\ME_TileMap.h"

enum EditorMode
{
    EDITOR_MODE_TILE,
    EDITOR_MODE_COLLISION_RECT,
    EDITOR_MODE_ENTITY,
    EDITOR_MODE_SELECT_TILE,
    EDITOR_MODE_DELETE,
};

typedef struct TileSpace
{
    TileMap tileMap;
    SDL_Rect tileMapCursor;
    
} TileSpace;

#define MAX_ENTITY_COUNT 100
typedef struct EntitySpace
{
	SDL_Point entityPositions[MAX_ENTITY_COUNT];
	SDL_Rect entityCursor;
	u32 entityTag[MAX_ENTITY_COUNT];
	u32 entityCount;
	u32 currentEntityTag;
    
} EntitySpace;

#define MAX_COLLISION_RECTS 100
typedef struct CollisionSpace
{
    SDL_Rect collisionRects[MAX_COLLISION_RECTS];
    SDL_Rect collisionCursor;
    u32 highlightedColRectIndex;
    u32 rectCount;
    
} CollisionSpace;

typedef struct TileSheetDisplaySpace
{
    SDL_Texture *displayTexture;
    SDL_Rect displayRect;
    SDL_Rect tileSheetCursor;
    
} TileSheetDisplaySpace;

typedef struct Editor
{
    TileSpace tileSpace;
    CollisionSpace collisionSpace;
    EntitySpace entitySpace;
    
    TileSheetDisplaySpace tileSheetDisplay;
    
    SDL_Rect deleteCursor;
    
    u32 editorMode;
    u32 lastEditorMode;
    
} Editor;

#endif //_MICRO_EDITOR_H