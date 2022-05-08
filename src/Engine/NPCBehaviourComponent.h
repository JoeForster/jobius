#pragma once

#include "ComponentManager.h"

// Placeholdery component for entities using the first-pass OOP Behaviour Tree system
struct NPCBehaviourComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_NPCBHV; }

	NPCBehaviourComponent() {}
	NPCBehaviourComponent(EntityID playerEntity): m_PlayerEntity(playerEntity) {}

	// TODO_AI_SENSOR TEMP dirty direct player access
	EntityID m_PlayerEntity = INVALID_ENTITY_ID;
};
