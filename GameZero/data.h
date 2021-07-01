#ifndef DATA_H
#define DATA_H

//NOTE(abhicv): all asset resources game ever need
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

} Level;

//NOTE(abhicv): generating all map names from level id
#define TO_STRING(s) #s
#define LEVEL_MAP_DIR_TO_STRING(levelName, mapType) TO_STRING(data/levels/levelName##.##mapType)

#define LEVEL_MAP(levelName) {\
.entityMapFile = LEVEL_MAP_DIR_TO_STRING(levelName, emap),\
.collisionMapFile = LEVEL_MAP_DIR_TO_STRING(levelName, cmap),\
.tileMapFile = LEVEL_MAP_DIR_TO_STRING(levelName, tmap),\
.tileMapTexture = NULL,\
}\

enum State
{
    SHOOT,
    PATROL,
    CHASE,
};

#endif //DATA_H
