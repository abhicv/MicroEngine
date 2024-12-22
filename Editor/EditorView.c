#include "MicroEditor.h"
#include "..\Engine\src\MicroECS.h"
#include "..\Engine\src\MicroUI.h"
#include "style.h"

#define MUI_ORIGIN_ID 2023

global TileMap tileMap;
global TileSheetDisplaySpace tileSheetDisplay;
global CollisionSpace collisionSpace;
global EntitySpace entitySpace;

global SDL_Rect deleteCursor;
global SDL_Rect tileMapCursor;
global SDL_Rect *activeCursor = 0;

// editor modes
global u32 editorMode = EDITOR_MODE_TILE;
global u32 lastEditorMode = EDITOR_MODE_TILE;
global bool disableEditorUI = false;

#define MAX_FILE_NAME_SIZE 100
global u8 tileMapFileName[MAX_FILE_NAME_SIZE] = "tmp.tmap";
global u8 collisionMapFileName[MAX_FILE_NAME_SIZE] = "tmp.cmap";
global u8 entityMapFileName[MAX_FILE_NAME_SIZE] = "tmp.emap";

void InitializeEditorData(SDL_Renderer *renderer, u32 displayWidth)
{
    tileMapCursor.x = 0;
    tileMapCursor.y = 0;
    tileMapCursor.w = tileMap.tileWidth;
    tileMapCursor.h = tileMap.tileHeight;
    deleteCursor = tileMapCursor;

    activeCursor = &tileMapCursor;
    
    tileMap.tileSheetTexture = IMG_LoadTexture(renderer, "assets/Sprites/tiles.png");
    
    SDL_Rect rect = {0};
    SDL_QueryTexture(tileMap.tileSheetTexture, NULL, NULL, &rect.w, &rect.h);
    
    tileSheetDisplay.displayRect.w = rect.w;
    tileSheetDisplay.displayRect.h = rect.h;
    tileSheetDisplay.displayRect.x = displayWidth - tileSheetDisplay.displayRect.w;
    tileSheetDisplay.displayRect.y = 0;
    
    tileSheetDisplay.tileSheetCursor.x = tileSheetDisplay.displayRect.x;
    tileSheetDisplay.tileSheetCursor.y = tileSheetDisplay.displayRect.y;
    tileSheetDisplay.tileSheetCursor.w = tileMap.tileSheetTileWidth;
    tileSheetDisplay.tileSheetCursor.h = tileMap.tileSheetTileHeight;
    
    tileSheetDisplay.displayTexture = tileMap.tileSheetTexture;
    
    collisionSpace.collisionCursor = tileMapCursor;
    collisionSpace.highlightedColRectIndex = MAX_COLLISION_RECTS;
    
    entitySpace.entityCursor = tileMapCursor;
    entitySpace.currentEntityTag = ENTITY_TAG_PLAYER;
}

char *EditModeToString(u32 mode)
{
    char *modeString = NULL;
    
    switch (mode)
    {
		case EDITOR_MODE_TILE:
        modeString = "Mode : Tile";
        break;
        
		case EDITOR_MODE_ENTITY:
        modeString = "Mode : Entity";
        break;
        
		case EDITOR_MODE_COLLISION_RECT:
        modeString = "Mode : Collision";
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        modeString = "Mode : Select";
        break;
        
        case EDITOR_MODE_DELETE:
        modeString = "Mode : Delete";
        break;
    }
    
    return modeString;
}

char *EntityTagToString(u32 tag)
{
	char *tagString = "NONE";
    
	switch(tag)
	{
		case ENTITY_TAG_PLAYER:
		tagString = "PLAYER";
		break;
        
		case ENTITY_TAG_LIZARD:
		tagString = "LIZARD";
		break;
        
        case ENTITY_TAG_SLIME:
        tagString = "SLIME";
        break;
        
        case ENTITY_TAG_FLYEE:
        tagString = "FYLYEE";
        break;
        
        case ENTITY_TAG_COIN:
        tagString = "COIN";
        break;
        
        case ENTITY_TAG_CAPTURE_FLAG:
        tagString = "FLAG";
        break;
        
        case ENTITY_TAG_CRATE:
        tagString = "CRATE";
        break;
	}
    
	return tagString;
}

b32 IsMouseOverSDLRect(u32 mouseX, u32 mouseY, SDL_Rect rect)
{
    b32 result = false;
    
    if(mouseX > rect.x && mouseX < (rect.x + rect.w) &&
       mouseY > rect.y && mouseY < (rect.y + rect.h))
    {
        result = true;
    }
    
    return result;
}

void CursorMovementControl(SDL_Rect *cursor, u32 deltaStep, SDL_Event *event)
{
    switch(event->type)
    {
        case SDL_KEYDOWN:
        switch(event->key.keysym.sym)
        {
            case SDLK_UP:
            cursor->y -= deltaStep;
            break;
            
            case SDLK_DOWN:
            cursor->y += deltaStep;
            break;
            
            case SDLK_LEFT:
            cursor->x -= deltaStep;
            break;
            
            case SDLK_RIGHT:
            cursor->x += deltaStep;
            break;
        }
    }
}

void CursorSizeControl(SDL_Rect *cursor, u32 deltaSize, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_w:
            cursor->h -= deltaSize;
            break;
            
            case SDLK_s:
            cursor->h += deltaSize;
            break;
            
            case SDLK_a:
            cursor->w -= deltaSize;
            break;
            
            case SDLK_d:
            cursor->w += deltaSize;
            break;
        }
    }
}

void PlaceTile()
{
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
                u32 index = xIndex + (yIndex * tileMap.tileMapWidth);

                // printf("placing at index: %d\n", index);

                Tile tile = {0};
                tile.tileMapPosX = x;
                tile.tileMapPosY = y;
                tile.tileSheetPosX = tileSheetDisplay.tileSheetCursor.x - tileSheetDisplay.displayRect.x;
                tile.tileSheetPosY = tileSheetDisplay.tileSheetCursor.y - tileSheetDisplay.displayRect.y;

                tileMap.tiles[index] = tile;
            }
        }
    }
}

void EditorHandleEvent(SDL_Event *event, MUI ui, MUI_Input *uiInput)
{
    MUI_GetInput(uiInput, event);
    
    switch(editorMode)
    {
        case EDITOR_MODE_TILE:
        {
            activeCursor = &tileMapCursor;
            CursorSizeControl(&tileMapCursor, tileMap.tileWidth, event);

            if(uiInput->leftMouseButtonDown && MUI_IdEqual(ui.hotWidgetId, MUI_NullId()))
            {
                PlaceTile();
            }            
        }
        break;
        
        case EDITOR_MODE_COLLISION_RECT:
        {
            activeCursor = &collisionSpace.collisionCursor;
            CursorSizeControl(&collisionSpace.collisionCursor, tileMap.tileWidth, event);
        }
        break;
        
        case EDITOR_MODE_ENTITY:
        {
            activeCursor = &entitySpace.entityCursor;
        }
        break;
        
        case EDITOR_MODE_SELECT_TILE:
        {
            CursorMovementControl(&tileSheetDisplay.tileSheetCursor, tileSheetDisplay.tileSheetCursor.w, event);
        }
        break;
        
        case EDITOR_MODE_DELETE:
        {
            activeCursor = &deleteCursor;
            CursorSizeControl(&deleteCursor, tileMap.tileWidth, event);
        }
        
        break;
    }

    u32 column = (u32)uiInput->mouseX / tileMap.tileWidth;
    u32 row = (u32)uiInput->mouseY / tileMap.tileHeight;

    activeCursor->x = column * tileMap.tileWidth;
    activeCursor->y = row * tileMap.tileHeight;

    switch (event->type)
    {
        case SDL_KEYDOWN:
        switch (event->key.keysym.sym)
        {
            case SDLK_TAB:
            if(editorMode == EDITOR_MODE_TILE)
            {
                editorMode = EDITOR_MODE_SELECT_TILE;
            }
            else if(editorMode == EDITOR_MODE_ENTITY)
            {
                entitySpace.currentEntityTag = entitySpace.currentEntityTag < (1 << 6) ? 
                                                    (entitySpace.currentEntityTag << 1) : 1;
            }
            break;
            
            case SDLK_LSHIFT:
            disableEditorUI = !disableEditorUI;
            break;

            case SDLK_SPACE:
            switch (editorMode)
            {                
                case EDITOR_MODE_COLLISION_RECT:
                collisionSpace.collisionRects[collisionSpace.rectCount] = collisionSpace.collisionCursor;
                collisionSpace.rectCount++;
                break;
                
                case EDITOR_MODE_ENTITY:
                entitySpace.entityPositions[entitySpace.entityCount].x = entitySpace.entityCursor.x + entitySpace.entityCursor.w / 2;
                entitySpace.entityPositions[entitySpace.entityCount].y = entitySpace.entityCursor.y + entitySpace.entityCursor.h / 2;
                entitySpace.entityTag[entitySpace.entityCount] = entitySpace.currentEntityTag;
                entitySpace.entityCount++;
                break;
                
                case EDITOR_MODE_DELETE:
                switch(lastEditorMode)
                {
                    case EDITOR_MODE_COLLISION_RECT:
                    for(u32 n = 0; n < collisionSpace.rectCount; n++)
                    {
                        i32 x = collisionSpace.collisionRects[n].x;
                        i32 y = collisionSpace.collisionRects[n].y;
                        
                        if(x >= deleteCursor.x && y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) && y <= (deleteCursor.y + deleteCursor.h))
                        {
                            collisionSpace.collisionRects[n] = collisionSpace.collisionRects[collisionSpace.rectCount - 1];
                            collisionSpace.rectCount -= 1;
                        }
                    }
                    break;
                    
                    case EDITOR_MODE_ENTITY:
                    for(u32 n = 0; n < entitySpace.entityCount; n++)
                    {
                        i32 x = entitySpace.entityPositions[n].x;
                        i32 y = entitySpace.entityPositions[n].y;
                        
                        if(x >= deleteCursor.x && y >= deleteCursor.y &&
                           x <= (deleteCursor.x + deleteCursor.w) &&
                           y <= (deleteCursor.y + deleteCursor.h))
                        {
                            entitySpace.entityPositions[n] = entitySpace.entityPositions[entitySpace.entityCount - 1];
                            entitySpace.entityTag[n] = entitySpace.entityTag[entitySpace.entityCount - 1];
                            entitySpace.entityCount -= 1;
                        }
                    }
                    break;
                }
                break;
            }
            break;
            
            case SDLK_DELETE:
            lastEditorMode = editorMode;
            editorMode = EDITOR_MODE_DELETE;
            break;
        }
        break;
        
        case SDL_KEYUP:
        switch (event->key.keysym.sym)
        {
            case SDLK_TAB:
            if (editorMode == EDITOR_MODE_SELECT_TILE)
            {
                editorMode = EDITOR_MODE_TILE;
            }
            break;
        }
        break;
    }
}

void EditorUpdateAndRender(SDL_Renderer *renderer, MUI ui, MUI_Input uiInput)
{
    MUI_BeginFrame(&ui, &uiInput);
    
    // entity name text ui
    for(u32 i = 0; i < entitySpace.entityCount; i++)
    {
        MUI_Rect rect = {0};
        rect.width = entitySpace.entityCursor.w;
        rect.height = entitySpace.entityCursor.h;
        rect.x = entitySpace.entityPositions[i].x - rect.width / 2;
        rect.y = (entitySpace.entityPositions[i].y - rect.height - 10);
        MUI_Text(&ui, GEN_MUI_IDi(i), rect, EntityTagToString(entitySpace.entityTag[i]), 18, textStyle, MUI_TEXT_ALIGN_MIDDLE);
    }
    
    // checking for collision rects mouse over
    if(editorMode == EDITOR_MODE_COLLISION_RECT)
    {
        collisionSpace.highlightedColRectIndex = MAX_COLLISION_RECTS;
        for(u32 i = 0; i < collisionSpace.rectCount; i++)
        {
            if(IsMouseOverSDLRect(uiInput.mouseX, uiInput.mouseY, collisionSpace.collisionRects[i]))
            {
                collisionSpace.highlightedColRectIndex = i;
                break;
            }
        }
    }
    
    const displayWidth = 1600;
    MUI_Rect rect = {displayWidth - 220, 500, 200, 40};
    MUI_PushColumnLayout(&ui, rect, 8);
    {
        MUI_TextA(&ui, GEN_MUI_ID(), EntityTagToString(entitySpace.currentEntityTag), 20, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        MUI_TextA(&ui, GEN_MUI_ID(), EditModeToString(editorMode), 20, textStyle, MUI_TEXT_ALIGN_MIDDLE);
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "tilemap", buttonStyle))
        {
            editorMode = EDITOR_MODE_TILE;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "collision box", buttonStyle))
        {
            editorMode = EDITOR_MODE_COLLISION_RECT;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "entity", buttonStyle))
        {
            editorMode = EDITOR_MODE_ENTITY;
        }
        
        if(MUI_ButtonA(&ui, GEN_MUI_ID(), "save file", buttonStyle))
        {
            WriteTileMapDataToFile(&tileMap, tileMapFileName);
            WriteCollisionDataToFile(&collisionSpace, collisionMapFileName);
            WriteEntityDataToFile(&entitySpace, entityMapFileName);
            printf("Data written to files %s, %s, and %s\n", tileMapFileName, collisionMapFileName, entityMapFileName);
        }
    }
    MUI_PopLayout(&ui);
    
    ME_RenderTileMap(&tileMap, renderer);
    RenderEditorSpace(renderer, tileMapCursor, tileMap, tileSheetDisplay);
    RenderTileSheetDisplay(&tileSheetDisplay, renderer);
    RenderCollisionSpace(&collisionSpace, renderer);
    RenderEntitySpace(&entitySpace, renderer);

    // drawing cursor
    ME_RenderDrawRect(renderer, activeCursor, (SDL_Color){255, 255, 255, 255});

    if(disableEditorUI) 
    {
        ui.hotWidgetId = MUI_NullId();
        ui.activeWidgetId = MUI_NullId();
    }

    MUI_EndFrame(&ui, renderer);
}

void EditorLoop(SDL_Renderer *renderer, u32 *appMode)
{
    bool quit = false;
    
    SDL_Event event = {0};

    MUI ui = {0};
    ui.fontFile = "assets/Font/OCRAEXT.ttf";

    MUI_Input uiInput = {0};

    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                (*appMode) = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    (*appMode) = APP_MODE_QUIT;
                }
                else if (event.key.keysym.sym == SDLK_r)
                {
                    WriteTileMapDataToFile(&tileMap, "tmp.tmap");
                    WriteCollisionDataToFile(&collisionSpace, "tmp.cmap");
                    WriteEntityDataToFile(&entitySpace, "tmp.emap");
                    (*appMode) = APP_MODE_GAME_TEST;
                    quit = true;
                }
                break;
            }
            EditorHandleEvent(&event, ui, &uiInput);
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        EditorUpdateAndRender(renderer, ui, uiInput);
        SDL_RenderPresent(renderer);
    }
}
