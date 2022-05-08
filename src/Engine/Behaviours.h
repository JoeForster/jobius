#pragma once

#include <iostream>

#include "BehaviourTree.h"

// TODO confirm whether we even need a base Action/Condition

class Action : public Behaviour
{
public:
	Action(Behaviour* parent, BehaviourTreeState& treeState)
	: Behaviour(parent, treeState) {}

protected:
	void OnInitialise() override
	{
		m_Status = BehaviourStatus::RUNNING;
	}
};

class Condition : public Behaviour
{
public:
	Condition(Behaviour* parent, BehaviourTreeState& treeState)
		: Behaviour(parent, treeState) {}

};
