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

static constexpr Vector2f s_GridSize { 20.0f, 20.0f };

static Vector2i WorldToGrid(const Vector3f& fromCoords)
{
	return Vector2i((int)(fromCoords.x/s_GridSize.x), (int)(fromCoords.y/s_GridSize.y)); 
}
static Vector3f GridToWorld(const Vector2i& fromCoords)
{
	return Vector3f((float)fromCoords.x*s_GridSize.x, (float)fromCoords.y*s_GridSize.y, 0.0f); 
}

