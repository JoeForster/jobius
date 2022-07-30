#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct GridTransformComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_GRIDTRANSFORM; }

	GridTransformComponent()
		: m_Pos{ 0, 0 } {}

	GridTransformComponent(Vector2i inPos)
		: m_Pos(inPos) {}

	Vector2i m_Pos;
	// TODO rotation or facing direction? Otherwise rename to GridPosComponent
};
