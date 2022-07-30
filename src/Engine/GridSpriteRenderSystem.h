#pragma once

#include <vector>

#include "System.h"

class GridSpriteRenderSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_GRIDRENDER; }

	GridSpriteRenderSystem(std::shared_ptr<World> parentWorld)
	: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser&) override;
	void Render_Main() final override;

private:
	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
