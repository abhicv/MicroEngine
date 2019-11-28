#include "../../include/MicroEngine/ME_Vector2D.h"

Vector2 NewVector2(DATATYPE x,DATATYPE y)
{
	Vector2 v;

	v.x = x;
	v.y = y;

	return v;
}

DATATYPE Vector2Mag(Vector2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

DATATYPE Vector2SqrMag(Vector2 v)
{
	return (v.x * v.x + v.y * v.y);
}

DATATYPE Vector2Dot(Vector2 v1,Vector2 v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

void Vector2Normalize(Vector2 *v)
{
	DATATYPE mag = Vector2Mag(*v);

	v->x /= mag;
	v->y /= mag;
}

void Vector2Scale(Vector2 *v,DATATYPE scalar)
{
	v->x *= scalar;
	v->y *= scalar;
}

Vector2 Vector2Subtract(Vector2 v1,Vector2 v2)
{
	Vector2 v;

	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;

	return v;
}
