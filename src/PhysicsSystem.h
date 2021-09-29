#pragma once

#include <vector>

#include "System.h"

class PhysicsSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_PHYSICS; }

	PhysicsSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:

};

