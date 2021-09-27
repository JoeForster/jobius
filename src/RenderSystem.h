#pragma once

#include <vector>

#include "System.h"

class SDLRenderManager;

class RenderSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_RENDER; }

	RenderSystem(std::shared_ptr<World> parentWorld)
	: System(parentWorld)
	{
	}

	void SetRenderManager(std::shared_ptr<SDLRenderManager> renderMan);
	void Init() override;
	void Render() final override;

private:

	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
