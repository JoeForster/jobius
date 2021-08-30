#pragma once

#include <vector>

#include "System.h"

class PhysicsSystem : public System
{
public:
	constexpr SystemType GetSystemType() { return SystemType::ST_PHYSICS; }

	PhysicsSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init() final override
	{
	}

	void Update(float deltaSecs) final override;

	void Render() final override
	{
	}

	void Destroy() final override
	{
	}

private:

};

