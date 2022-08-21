#pragma once

#include "ComponentManager.h"

#include "Vector.h"
#include "Coordinates.h"

struct GridWorldComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_GRIDWORLD; }

	GridWorldComponent() {}
	GridWorldComponent(Rect2f bounds, int gridSize)
	: m_Bounds(bounds)
	, m_GridSize(gridSize) {}

	Rect2f m_Bounds; // TODO should be 3D and add min/max vector helpers
	int m_GridSize;

	// Access helpers
	
	Vector2i WorldToGrid(const Vector3f& fromCoords) const
	{
		return Vector2i((int)(fromCoords.x/m_GridSize), (int)(fromCoords.y/m_GridSize)); 
	}
	Vector3f GridToWorld(const Vector2i& fromCoords) const
	{
		return Vector3f((float)(fromCoords.x*m_GridSize), (float)(fromCoords.y*m_GridSize), 0.0f); 
	}
	
	Vector2i GridToScreen(const Vector2i& fromCoords) const
	{
		// TODO optimise
		return WorldToScreen(GridToWorld(fromCoords));
	}
};
