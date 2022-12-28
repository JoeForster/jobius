#pragma once

#include "ComponentManager.h"

#include "Vector.h"

// TODO rename to 2D?
struct AABBComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_AABB; }

	AABBComponent() {}

	AABBComponent(Vector2f box, Vector2f offset)
		: m_Box(Rect2f::FromBoxAndOffset(box, offset)) {}

	Rect2f m_Box;
};
