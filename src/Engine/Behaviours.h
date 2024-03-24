#pragma once

#include <iostream>

#include "BehaviourStatus.h"
#include "BehaviourTree.h"

class Decorator : public Behaviour
{
public:
	Decorator(Behaviour* parent, BehaviourTreeData& treeData)
		: Behaviour(parent, treeData)
		, m_Child(nullptr) {}

	size_t GetChildCount() const final { return 1; }
	const Behaviour* GetChildAt(size_t index) const final { return (index == 0 ? m_Child : nullptr); }
	void AddChild(Behaviour* child) override { assert(m_Child == nullptr); m_Child = child; }

protected:
	virtual void OnInitialise() override;

	Behaviour* m_Child;
};

// Repeat decorator: call the child N times immediately, if it succeeds.
class Repeat : public Decorator
{
public:
	Repeat(Behaviour* parent, BehaviourTreeData& treeData, unsigned numRepeats)
		: Decorator(parent, treeData)
		, m_NumRepeats(numRepeats)
	{}

protected:
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;

private:
	unsigned m_RepeatCounter = 0;
	unsigned m_NumRepeats;
};


class Composite : public Behaviour
{
public:
	Composite(Behaviour* parent, BehaviourTreeData& treeData)
	: Behaviour(parent, treeData) {}

	virtual void AddChild(Behaviour*) override;
	void RemoveChild(Behaviour*);
	void ClearChildren();

	// TODO validate index OR assert consistently when out of bounds
	size_t GetChildCount() const final { return m_Children.size(); }
	const Behaviour* GetChildAt(size_t index) const final { return m_Children[index]; }

protected:
	// TODO can we avoid using raw pointers here?
	typedef std::vector<Behaviour*> Behaviours;
	Behaviours m_Children;
};

// Sequence node: run each child in order.
class Sequence : public Composite
{
public:
	Sequence(Behaviour* parent, BehaviourTreeData& treeData)
	: Composite(parent, treeData) {}

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;

private:
	Behaviours::iterator m_CurrentChild;
};

class Filter : public Sequence
{
public:
	Filter(Behaviour* parent, BehaviourTreeData& treeData)
	: Sequence(parent, treeData) {}

	void AddCondition(Behaviour* condition);
	void AddAction(Behaviour* action);
};


// Parallel node: run through each child, returning success/failure based on the corresponding Policy value.
class Parallel : public Composite
{
public:
	enum class Policy
	{
		RequireOne,
		RequireAll
	};

	Parallel(Behaviour* parent, BehaviourTreeData& treeData, Policy success, Policy failure)
		: Composite(parent, treeData)
		, m_SuccessPolicy(success)
		, m_FailurePolicy(failure)
	{
	}

protected:
	Policy m_SuccessPolicy;
	Policy m_FailurePolicy;

	virtual BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;
};

// Monitor node: a parallel node which will always re-check a set of conditions befor eexecuting the actions.
// Children sequence is a set of conditions first, then the actions.
class Monitor : public Parallel
{
public:
	Monitor(Behaviour* parent, BehaviourTreeData& treeData)
		: Parallel(parent, treeData, Policy::RequireOne, Policy::RequireOne)
	{}

	void AddCondition(Behaviour* condition);
	void AddAction(Behaviour* action);

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;
};

// Base "passive" selector: tick children until one succeeds or runs.
class Selector : public Composite
{
public:
	Selector(Behaviour* parent, BehaviourTreeData& treeData);

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) override;

protected:
	Behaviours::iterator m_CurrentChild;
};

// Active selector: like a passive selector but keeps re-checking higher-priority (leftmost) children before moving on to the next.
class ActiveSelector : public Selector
{
public:
	ActiveSelector(Behaviour* parent, BehaviourTreeData& treeData)
		: Selector(parent, treeData)
	{}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

protected:
	BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) final;
};

class Action : public Behaviour
{
public:
	Action(Behaviour* parent, BehaviourTreeData& treeData)
	: Behaviour(parent, treeData) {}

protected:
	void OnInitialise() override
	{
		m_Status = BehaviourStatus::RUNNING;
	}
};

class Condition : public Behaviour
{
public:
	Condition(Behaviour* parent, BehaviourTreeData& treeData)
		: Behaviour(parent, treeData) {}

};
