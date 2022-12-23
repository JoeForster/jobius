#pragma once

// Coordinate helpers
// TODO: Needs thinking architecturally best place for this - subclass of world for 2D might make sense?
// NOTE: "Screen" is slightly wrong term here as it's pre-zoom and zoom is handled within the SDLRenderManager 
// TODO: Consider proper types for coordinates, can use for type-safe draw calls at world/screen pos?

#include "Vector.h"

// TODO move implenentation into cpp with forward declarations
#include "World.h"
#include "GridWorldComponent.h"
#include "Camera2DComponent.h"

namespace WorldCoords
{
	static Vector3f ScreenToWorld(const Vector2f& cameraPos, const Vector2i& fromCoords)
	{
		Vector3f worldCoords((float)fromCoords.x, (float)fromCoords.y, 0.0f);
		worldCoords.x += cameraPos.x;
		worldCoords.y += cameraPos.y;
		return worldCoords;
	}

	static Vector2i WorldToScreen(const Vector2f& cameraPos, const Vector3f& fromCoords)
	{
		Vector2i screenCoords((int)fromCoords.x, (int)fromCoords.y);
		screenCoords.x -= (int)cameraPos.x;
		screenCoords.y -= (int)cameraPos.y;
		return screenCoords;
	}

	// 2D World Helpers

	static Vector3f ScreenToWorld(World& world, const Vector2i& fromCoords)
	{
		const auto& camera = world.GetGlobalComponent<Camera2DComponent>();
		return WorldCoords::ScreenToWorld(camera.m_CameraPos, fromCoords);
	}

	static Vector2i WorldToScreen(World& world, const Vector3f& fromCoords)
	{
		// (depends on both gridworld and camera so maybe should be in a better location - 2D world subclass?)
		const auto& camera = world.GetGlobalComponent<Camera2DComponent>();
		return WorldCoords::WorldToScreen(camera.m_CameraPos, fromCoords);
	}

	// 2D Grid World Helpers

	static Vector2i WorldToGrid(World& world, const Vector3f& fromCoords)
	{
		const auto& grid = world.GetGlobalComponent<GridWorldComponent>();
		return Vector2i((int)(fromCoords.x / grid.m_GridSize), (int)(fromCoords.y / grid.m_GridSize));
	}

	static Vector3f GridToWorld(World& world, const Vector2i& fromCoords)
	{
		const auto& grid = world.GetGlobalComponent<GridWorldComponent>();
		return Vector3f((float)(fromCoords.x * grid.m_GridSize), (float)(fromCoords.y * grid.m_GridSize), 0.0f);
	}

	static Vector2i GridToScreen(World& world, const Vector2i& fromCoords)
	{
		// TODO optimise/tidy
		return WorldToScreen(world, GridToWorld(world, fromCoords));
	}
}
