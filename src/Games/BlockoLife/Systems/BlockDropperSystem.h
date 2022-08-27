#pragma once

#include "System.h"

class BlockDropperSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_RENDER; }

	BlockDropperSystem(std::shared_ptr<World> parentWorld)
	: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser&) override;
	void Update(float deltaSecs) final override;
	void Render_Main() final override;

private:
	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
