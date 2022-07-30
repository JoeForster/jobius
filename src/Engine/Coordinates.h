#pragma once

#include "Vector.h"

// Quick/temp helpers for converting coordinates
// TODO: Consider proper types for coordinates, can use for type-safe draw calls at world/screen pos?

static Vector3f ScreenToWorld(const Vector2i& fromCoords)
{
	return Vector3f((float)fromCoords.x, (float)fromCoords.y, 0.0f); 
}

static Vector2i WorldToScreen(const Vector3f& fromCoords)
{
	return Vector2i((int)fromCoords.x, (int)fromCoords.y); 
}
