#pragma once

#include "ComponentManager.h"

// Placeholdery component for entities using the first-pass OOP Behaviour Tree system
struct NPCBehaviourComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_NPCBHV; }

	NPCBehaviourComponent() {}

	// TODO any state
};
