#pragma once

#include "System.h"

class BehaviourSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_BEHAVIOUR; }

	BehaviourSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;
	void Render_Debug() final override;

private:

};
