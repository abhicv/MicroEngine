#include "MicroPhysics.h"

static MP_DynamicObject *dynObjects;
static MP_StaticObject *statObjects;

MP_DynamicObject MP_CreateDynamic(float mass, SDL_Rect collisionRect) {
    MP_DynamicObject dObject;

    dObject.objectID = rand() % 10000;
    dObject.mass = mass;
    dObject.velocity = NewVector2(0, 0);
    dObject.collisionRect = collisionRect;

    return dObject;
}

void MP_UpdateDynamicObj(MP_DynamicObject *dObj) {

}
