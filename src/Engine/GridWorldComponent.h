#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct GridWorldComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_GRIDWORLD; }

	GridWorldComponent() {}
	GridWorldComponent(Rect2f bounds, int gridSize)
	: m_Bounds(bounds)
	, m_GridSize(gridSize) {}

	Rect2f m_Bounds { Vector2f::ZERO, Vector2f::ZERO }; // TODO should be 3D and add min/max vector helpers
	int m_GridSize = 0;
};
