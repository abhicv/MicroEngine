#ifndef _FILE_H
#define _FILE_H

#include "stdio.h"

#include "MicroEditor.h"

#include "..\Engine\src\types.h"
#include "..\Engine\src\ME_TileMap.h"

CollisionSpace LoadCollisionMap(const char *fileName);
EntitySpace LoadEntityMap(const char *fileName);

void WriteTileMapDataToFile(TileMap *tileMap, const char *fileName);
void WriteCollisionDataToFile(CollisionSpace *colSpace, const char *fileName);
void WriteEntityDataToFile(EntitySpace *entitySpace, const char *fileName);

#endif //_FILE_H
