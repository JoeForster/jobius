#pragma once

#include "ComponentManager.h"
#include "RenderTypes.h"

struct SpriteComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_SPRITE; }

	SpriteComponent()
		: m_SpriteID{ ResourceID_Invalid } {}

	SpriteComponent(ResourceID spriteID)
		: m_SpriteID(spriteID) {}

	ResourceID m_SpriteID;
};
