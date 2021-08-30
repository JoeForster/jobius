#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(ResourceID spriteRes)
	: m_ResourceID(spriteRes)
{
}

void SpriteRenderer::Render(SDLRenderManager& renderMgr, int x, int y)
{
	if (m_ResourceID == SDLRenderManager::ResourceID_Invalid)
	{
		return;
	}

	renderMgr.Draw(m_ResourceID, x, y);
}