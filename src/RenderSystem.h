#pragma once

#include <vector>

#include "System.h"

class SDLRenderManager;

struct RenderSystemInitialiser : public SystemInitialiser
{
	std::shared_ptr<SDLRenderManager> m_RenderMan;
};

class RenderSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_RENDER; }

	RenderSystem(std::shared_ptr<World> parentWorld)
	: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser&) override;
	void Render() final override;

private:

	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
