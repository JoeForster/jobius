#pragma once

#include <iostream>

#include "BehaviourTree.h"

// TODO confirm whether we even need a base Action/Condition

class Action : public Behaviour
{
public:
	Action(Behaviour* parent, const BehaviourTreeState& treeState)
	: Behaviour(parent, treeState) {}

protected:
	void OnInitialise() override
	{
		m_Status = BehaviourStatus::RUNNING;
	}

	// TODO status terminate?
	//void OnTerminate(BehaviourStatus) override
	//{
	//
	//}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override
	{
		stream << "Action[Status:" << StatusString[(int)m_Status] << "]";
		return stream;
	}

private:
};

class Condition : public Behaviour
{
public:
	Condition(Behaviour* parent, const BehaviourTreeState& treeState)
		: Behaviour(parent, treeState) {}

protected:

	virtual std::ostream& DebugToStream(std::ostream& stream) const override
	{
		stream << "Condition[Status:" << StatusString[(int)m_Status] << "]";
		return stream;
	}

};
