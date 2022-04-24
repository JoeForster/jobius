#pragma once

#include "System.h"

class PlayerControlSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_PLAYERCONTROL; }

	PlayerControlSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:

};
