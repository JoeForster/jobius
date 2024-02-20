#pragma once

#include "Vector.h"

#include "ComponentManager.h"

// Placeholdery component to hold state and working knowledge data neded for NPC behaviour/control;
// Bit of a mess as we don't have a proper sensor system or flow yet - NPCControlSystem writes initial state
// and then the behaviour tree will read/write values willy nilly
// TODO need a knowledge system, and define how the ECS data will be fed into BT without
// the need for game-specific concepts to bleed into engine
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
