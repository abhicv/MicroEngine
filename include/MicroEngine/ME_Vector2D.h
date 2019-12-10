#ifndef ME_VECTOR2D_H
#define ME_VECTOR2D_H

#include <math.h>

typedef float DATATYPE;

typedef struct VECTOR2D
{
	DATATYPE x,y;

} Vector2;

Vector2 NewVector2(DATATYPE,DATATYPE);
DATATYPE Vector2Mag(Vector2);
DATATYPE Vector2SqrMag(Vector2);
DATATYPE Vector2Dot(Vector2 ,Vector2);
void Vector2Normalize(Vector2*);
void Vector2Scale(Vector2* ,DATATYPE);
Vector2 Vector2Subtract(Vector2 ,Vector2);
Vector2 ReflectDir(Vector2,Vector2);

#endif //ME_VECTOR2D_H
