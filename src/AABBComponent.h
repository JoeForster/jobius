#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct AABBComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_AABB; }

	AABBComponent() {}

	AABBComponent(float width, float height)
		: m_Box{width, height} {}

	Vector2f m_Box;
};
