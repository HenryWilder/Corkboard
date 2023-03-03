#include "VectorMath.h"

Vector2 Vector2SetLength(Vector2 v, float newLength)
{
	const float l = Vector2Length(v);
	const float scale = newLength / l;
	const Vector2 vFinal = v * scale;

	return vFinal;
}

Vector2 Vector2SetDistance(Vector2 v, float newLength, Vector2 from)
{
	Vector2 vFinal = from + Vector2SetLength(v - from, newLength);

	return vFinal;
}
