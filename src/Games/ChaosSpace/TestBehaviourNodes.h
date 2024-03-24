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
	CheckPlayerVisible(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class IsPlayerInRange : public Condition
{
public:
	IsPlayerInRange(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class FireAtPlayer : public Action
{
public:
	FireAtPlayer(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class MoveToPlayersLKP : public Action
{
public:
	MoveToPlayersLKP(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class LookAround : public Action
{
public:
	LookAround(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class CheckHasPlayersLKP : public Condition
{
public:
	CheckHasPlayersLKP(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

class MoveToRandomPosition : public Action
{
public:
	MoveToRandomPosition(Behaviour* parent, BehaviourTreeData& treeData);
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};