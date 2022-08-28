#pragma once

#include "ComponentManager.h"
#include "RenderTypes.h"

struct SpriteComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_SPRITE; }
	static constexpr uint8_t DEFAULT_TEX_ALPHA = 255;


	SpriteComponent()
		: m_SpriteID{ ResourceID_Invalid }
		, m_TextureAlpha(DEFAULT_TEX_ALPHA) {}

	SpriteComponent(ResourceID spriteID, uint8_t textureAlpha = DEFAULT_TEX_ALPHA)
		: m_SpriteID(spriteID)
		, m_TextureAlpha(textureAlpha) {}

	ResourceID m_SpriteID;
	uint8_t m_TextureAlpha;
};
