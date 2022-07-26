#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct GridWorldComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_GRIDWORLD; }

	GridWorldComponent() {}
	GridWorldComponent(Rect2D bounds): m_Bounds(bounds) {}

	Rect2D m_Bounds;
};
