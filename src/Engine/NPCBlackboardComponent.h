#pragma once

#include "Vector.h"

#include "ComponentManager.h"

// Placeholdery component to control data needed by for the NPC controller
// General flow is: Sensor System updates -> Control System reads 
struct NPCBlackboardComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BLACKBOARD_NPC; }

	NPCBlackboardComponent() {}

	// XXX: Mixture of sensor values and behaviour values here, could we separate them more cleanly?
	Vector2f NPCPos {};
	Vector2f NPCTargetPos {};
	bool NPCTargetPosSet = false;
	// TODO_AI_MULTIPLE_PLAYERS
	Vector2f PlayerPos {};
	Vector2f PlayerLastKnownPos {};
	bool PlayerLastKnownPosSet = false;

	// TODO_AI_INTENT: This doesn't belong in the blackboard..
	bool NPCFired = false;
};
