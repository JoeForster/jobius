#pragma once

#include "System.h"

#include <map>

class Camera2DSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_CAMERA_2D; }

	Camera2DSystem(std::shared_ptr<World> parentWorld)
	: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;
	void Render_Main() final override;
	//void Render_Debug() final override;

private:
	std::shared_ptr<SDLRenderManager> m_RenderMan;
};
