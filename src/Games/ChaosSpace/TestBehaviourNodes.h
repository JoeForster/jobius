#pragma once

#include "Behaviours.h"

// TODO HACK move AIConfig into a global component or something at least

struct AIConfig
{
	float ArriveDistSq = 5.f * 5.f;
	float VisionRadiusSq = 200.f * 200.f;
	float AttackRadiusSq = 20.f * 20.f;
};

static AIConfig s_AIConfig;

class CheckPlayerVisible : public Condition
{
public:
	CheckPlayerVisible(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class IsPlayerInRange : public Condition
{
public:
	IsPlayerInRange(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class FireAtPlayer : public Action
{
public:
	FireAtPlayer(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class MoveToPlayersLKP : public Action
{
public:
	MoveToPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class LookAround : public Action
{
public:
	LookAround(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class CheckHasPlayersLKP : public Condition
{
public:
	CheckHasPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};

class MoveToRandomPosition : public Action
{
public:
	MoveToRandomPosition(Behaviour* parent, BehaviourTreeState& treeState);
	BehaviourStatus Update(NPCBlackboardComponent& blackboard) override;
};