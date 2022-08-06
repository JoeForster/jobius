#pragma once

#include "System.h"
#include "SDLRenderManager.h"

class GameOfLifeSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_BL_GAMEOFLIFE; }

	GameOfLifeSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}
	
	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:
	void Tick();
};
