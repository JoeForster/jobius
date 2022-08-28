#pragma once

#include "ComponentManager.h"

#include "Vector.h"
#include "World.h"

#include "Camera2DComponent.h"

struct GridWorldComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_GRIDWORLD; }

	GridWorldComponent() {}
	GridWorldComponent(Rect2f bounds, int gridSize)
	: m_Bounds(bounds)
	, m_GridSize(gridSize) {}

	Rect2f m_Bounds; // TODO should be 3D and add min/max vector helpers
	int m_GridSize;

	// Coordinate helpers
	// TODO: Needs thinking architecturally best place for this
	// - on a subclass of World for 2D or a global component OR some sort of world-based component?
	// NOTE: "Screen" is slightly misleading as it's pre-zoom and zoom is handled within the SDLRenderManager 
	// TODO: Consider proper types for coordinates, can use for type-safe draw calls at world/screen pos?

	Vector3f ScreenToWorld(World& world, const Vector2i& fromCoords) const
	{
		const auto& camera = world.GetGlobalComponent<Camera2DComponent>();
		Vector3f worldCoords((float)fromCoords.x, (float)fromCoords.y, 0.0f); 
		worldCoords.x += camera.m_CameraPos.x;
		worldCoords.y += camera.m_CameraPos.y;
		return worldCoords;
	}

	Vector2i WorldToScreen(World& world, const Vector3f& fromCoords) const
	{
		// (depends on both gridworld and camera so maybe should be in a better location - 2D world subclass?)
		const auto& camera = world.GetGlobalComponent<Camera2DComponent>();
		Vector2i screenCoords((int)fromCoords.x, (int)fromCoords.y);
		screenCoords.x -= (int)camera.m_CameraPos.x;
		screenCoords.y -= (int)camera.m_CameraPos.y;
		return screenCoords; 
	}
	
	Vector2i WorldToGrid(const Vector3f& fromCoords) const
	{
		return Vector2i((int)(fromCoords.x/m_GridSize), (int)(fromCoords.y/m_GridSize)); 
	}

	Vector3f GridToWorld(const Vector2i& fromCoords) const
	{
		return Vector3f((float)(fromCoords.x*m_GridSize), (float)(fromCoords.y*m_GridSize), 0.0f); 
	}
	
	Vector2i GridToScreen(World& world, const Vector2i& fromCoords) const
	{
		// TODO optimise
		return WorldToScreen(world, GridToWorld(fromCoords));
	}

};
