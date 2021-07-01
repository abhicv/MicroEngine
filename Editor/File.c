#include "File.h"

CollisionSpace LoadCollisionMap(const char *fileName)
{
    FILE *colMapFile = fopen(fileName, "r");

    CollisionSpace colSpace = {0};

    if(colMapFile != NULL)
    {
        u32 count = 0;
        fscanf(colMapFile, "n:%d\n", &count);

        for(u32 i = 0; i < count; i++)
        {
            fscanf(colMapFile, "c:%d,%d,%d,%d\n",
                   &colSpace.collisionRects[i].x,
                   &colSpace.collisionRects[i].y,
                   &colSpace.collisionRects[i].w,
                   &colSpace.collisionRects[i].h);
        }
        colSpace.rectCount = count;
        fclose(colMapFile);
    }

    return colSpace;
}

EntitySpace LoadEntityMap(const char *fileName)
{
    EntitySpace eSpace = {0};
    FILE *entityFile = fopen(fileName, "r");

    if(entityFile != NULL)
    {
        u32 entityCount = 0;
        fscanf(entityFile, "n:%d\n", &entityCount);

        for(u32 i = 0; i < entityCount; i++)
        {
            fscanf(entityFile, "e:%d,%d,%d\n",
                   &eSpace.entityTag[i],
                   &eSpace.entityPositions[i].x,
                   &eSpace.entityPositions[i].y);
        }

        eSpace.entityCount = entityCount;
        fclose(entityFile);
    }
    return eSpace;
}


void WriteTileMapDataToFile(TileMap *tileMap, const char *fileName)
{
    FILE *tileMapFile = fopen(fileName, "w");

    if(tileMapFile != NULL)
    {
        //header for tile map file
        fprintf(tileMapFile, "w:%d\n", tileMap->tileMapWidth);
        fprintf(tileMapFile, "h:%d\n", tileMap->tileMapHeight);
        fprintf(tileMapFile, "tw:%d\n", tileMap->tileWidth);
        fprintf(tileMapFile, "th:%d\n", tileMap->tileHeight);
        fprintf(tileMapFile, "sw:%d\n", tileMap->tileSheetTileWidth);
        fprintf(tileMapFile, "sh:%d\n", tileMap->tileSheetTileHeight);

        //writing tile data
        u32 i = 0;
        for(i = 0; i < tileMap->tileCount; i++)
        {
            fprintf(tileMapFile, "t:%d,%d,%d,%d\n",
                    tileMap->tiles[i].tileMapPosX,
                    tileMap->tiles[i].tileMapPosY,
                    tileMap->tiles[i].tileSheetPosX,
                    tileMap->tiles[i].tileSheetPosY);
        }
        fclose(tileMapFile);
    }
    else
    {
        printf("Could not write to file '%s'\n", fileName);
    }
}

void WriteCollisionDataToFile(CollisionSpace *colSpace, const char *fileName)
{
    if(colSpace->rectCount > 0)
    {
        FILE *colMapFile = fopen(fileName, "w");

        if(colMapFile != NULL)
        {
            fprintf(colMapFile, "n:%d\n", colSpace->rectCount);

            for(u32 i = 0; i < colSpace->rectCount; i++)
            {
                fprintf(colMapFile, "c:%d,%d,%d,%d\n",
                        colSpace->collisionRects[i].x,
                        colSpace->collisionRects[i].y,
                        colSpace->collisionRects[i].w,
                        colSpace->collisionRects[i].h);
            }
            fclose(colMapFile);
        }
    }
    else
    {
        printf("Could not write to file '%s'\n", fileName);
    }
}

void WriteEntityDataToFile(EntitySpace *entitySpace, const char *fileName)
{
    if(entitySpace->entityCount > 0)
    {
        FILE *entityFile = fopen(fileName, "w");

        if(entityFile != NULL)
        {
            fprintf(entityFile, "n:%d\n", entitySpace->entityCount);

            for(u32 i = 0; i < entitySpace->entityCount; i++)
            {
                fprintf(entityFile, "e:%d,%d,%d\n",
                        entitySpace->entityTag[i],
                        entitySpace->entityPositions[i].x,
                        entitySpace->entityPositions[i].y);
            }
            fclose(entityFile);
        }
        else
        {
            printf("Could not write to file '%s'\n", fileName);
        }
    }
}
