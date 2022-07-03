#pragma once

#include "System.h"

class BoxCollisionSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_BOXCOLLISION; }

	BoxCollisionSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser&) override;

	void Update(float deltaSecs) final override;
	void Render_Debug() final override;

private:
	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
