#pragma once

#include "System.h"

#include <map>

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
	void Render_Debug() final override;

private:
	std::shared_ptr<SDLRenderManager> m_RenderMan;

	// TODO_DEBUG_DRAW TODO_RESOURCE_MANAGEMENT - TEMP structure for debug - add a component for this instead, or should debug draw system manage centrally?
	typedef std::map<EntityID, ResourceID> EntityResMap;
	EntityResMap m_DebugText;
};
