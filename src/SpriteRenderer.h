#pragma once

#include "SDLRenderManager.h"

class SpriteRenderer
{
public:
	SpriteRenderer(ResourceID spriteRes);

	void Render(SDLRenderManager& renderMgr, int x, int y);

private:
	ResourceID m_ResourceID;
};
