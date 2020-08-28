#ifndef DATA_H
#define DATA_H

//NOTE(abhicv): all asset resources game needs 
typedef struct GameResource
{
    //entity sprite
    SDL_Texture *playerSprite;
    SDL_Texture *enemySprite;
    SDL_Texture *itemSprite;
    
    //tileSheet
    SDL_Surface *tileSheetSurface;
    
    //Audio
    ME_Audio coinSound;
    ME_Audio shootSound;
    ME_Audio jumpSound;
    ME_Audio hurtSound;
    
    bool loaded;
    
} GameResource;

typedef struct Level
{
    //NOTE(abhicv): map file names to load from disk
    char *entityMapFile;
    char *collisionMapFile;
    char *tileMapFile;
    
    SDL_Texture *tileMapTexture;
    
    bool completed;
} Level;

//NOTE(abhicv): generating all map names from level id
#define TO_STRING(s) #s
#define LEVE_MAP_DIR_TO_STRING(x, map) TO_STRING(data/levels/level##x##.##map)
#define LEVEL_MAP(index) [index] = {\
.entityMapFile = LEVE_MAP_DIR_TO_STRING(index, emap),\
.collisionMapFile = LEVE_MAP_DIR_TO_STRING(index, cmap),\
.tileMapFile = LEVE_MAP_DIR_TO_STRING(index, tmap),\
.tileMapTexture = NULL,\
}\

typedef struct GameState
{
    u32 score;
    u32 coinCount;
    
} GameState;

#endif //DATA_H
