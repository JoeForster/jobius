#pragma once

#include "System.h"

class InputSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_INPUT; }

	InputSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:

};
