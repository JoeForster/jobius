#pragma once

#include "ComponentManager.h"

// TODO rename to 2D?
struct BehaviourNodeComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BTNODE; }

	BehaviourNodeComponent() {}

	BehaviourNodeComponent(Vector2f box, Vector2f offset)
	: m_Box(Rect2f::FromBoxAndOffset(box, offset)) {}

	Rect2f m_Box;
};
