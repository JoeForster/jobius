#pragma once

#include "ComponentManager.h"

#include "Vector.h"

// TODO rename to 2D?
struct AABBComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_AABB; }

	AABBComponent() {}

	AABBComponent(Vector2f box, Vector2f offset)
		: m_Box(box), m_Offset(offset) {}

	// TODO replace with Rect2f
	Vector2f m_Box = Vector2f::ZERO;
	Vector2f m_Offset = Vector2f::ZERO;
};
