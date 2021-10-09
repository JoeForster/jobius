#pragma once

#include "System.h"

class SDLInputSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_INPUT; }

	SDLInputSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:

};
