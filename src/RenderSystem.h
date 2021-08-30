#pragma once

#include <vector>

#include "System.h"
#include "SpriteRenderer.h"

class RenderSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_RENDER; }
	//virtual SystemType GetSystemType() const final override { return SystemType::ST_RENDER; }

	RenderSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init() final override
	{
	}

	void Update(float deltaSecs) final override
	{
	}

	void Render() final override;

	void Destroy() final override
	{
	}

private:

	std::vector<SpriteRenderer> m_Renderers;
};

