#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct AABBComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_AABB; }

	AABBComponent() {}

	AABBComponent(Vector2f box, Vector2f offset)
		: m_Box(box), m_Offset(offset) {}

	Vector2f m_Box = VECTOR2F_ZERO;
	Vector2f m_Offset = VECTOR2F_ZERO;
};
