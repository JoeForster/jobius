#pragma once

#include <vector>
#include <iostream>
#include <assert.h>

#include "BehaviourStatus.h"

// First attempt at a very very naive and unoptimised (first-generation, OOP) behaviour tree implementation.
// TODO Tidy-up pass required (files split, namespace, smurf, BStatus etc)

// Holder for state of behaviour tree accessible to nodes within
// (to prevent circular behaviour <--> tree dependency)
// May become an enum?
// TODO: some of this should be made const again, if we don't do away with this soon..
class Behaviour;
struct BehaviourTreeData;
struct NPCBlackboardComponent;

// Runtime state for a specific instance of a behaviour tree (i.e. per-NPC)
struct BehaviourTreeState
{
	bool IsStarted = false;
	const Behaviour* LastActiveNode = nullptr;
};


// The actual structure of a behaviour tree, built by BehaviourTreeBuilder
// NOTE this still contains some state within the behaviours, so it still has to be copied per owner.
struct BehaviourTreeData
{
	~BehaviourTreeData();

	Behaviour* m_Root = nullptr;
	std::vector<Behaviour*> m_Behaviours;
	bool IsStructureLocked = false;
};


class Behaviour
{
public:
	Behaviour(Behaviour* parent, BehaviourTreeData& treeData);
	virtual ~Behaviour() {}

	BehaviourStatus Tick(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard);
	void Abort();

	bool IsTerminated() const;
	bool IsRunning() const;

	inline BehaviourStatus GetStatus() const { return m_Status; }
	inline const Behaviour* GetParent() const { return m_Parent; }
	inline Behaviour* GetParent() { return m_Parent; }

	virtual size_t GetChildCount() const { return 0; }
	virtual const Behaviour* GetChildAt(size_t index) const { return nullptr; }
	// TEMP Construction - can be made unnecessary if builder knows whether leaf node at compile time (concepts)
	virtual void AddChild(Behaviour*) { assert(false && "Unsupported AddChild"); }

	virtual std::ostream& DebugToStream(std::ostream& stream) const;

protected:
	virtual void OnInitialise() {}
	virtual BehaviourStatus Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard) = 0;
	virtual void OnTerminate(BehaviourStatus) {}

	// TODO remove this so we can remove the dependency on BehaviourTreeData
	inline bool CanChangeStructure() const { return !m_TreeData.IsStructureLocked; }

	BehaviourStatus m_Status;
	Behaviour* m_Parent; // TODO use smart pointers?

private:
	BehaviourTreeData& m_TreeData;
};


namespace BehaviourTreeHelpers
{
	BehaviourStatus TickTree(NPCBlackboardComponent& blackboard, BehaviourTreeData& treeData, BehaviourTreeState& treeState);
	void StepTree();
	void StartTree(BehaviourTreeState& state);
	std::ostream& DebugOutTree(const Behaviour* root, std::ostream& stream);
}


// BEHAVIOUR TREE wrapper class
// - Owns the Behaviour memory once added
// - Tracks the state of the tree (locked/started)
// Note that ECS implementation has BehaviourTreeComponent instead. It shares code via BehaviourTreeHelpers

class BehaviourTreeInstance
{
public:
	BehaviourStatus Tick(NPCBlackboardComponent& blackboard) { return BehaviourTreeHelpers::TickTree(blackboard, m_Tree, m_State); }
	void Step() { BehaviourTreeHelpers::StepTree(); }
	void Start() { BehaviourTreeHelpers::StartTree(m_State); }

	inline Behaviour* GetRoot() { return m_Tree.m_Root; }
	inline BehaviourTreeState& GetState() { return m_State; }

	bool IsStructureLocked() const { return m_Tree.IsStructureLocked; }

	virtual std::ostream& DebugToStream(std::ostream& stream) const { return BehaviourTreeHelpers::DebugOutTree(m_Tree.m_Root, stream); }

private:
	// TODO rename these (and rename BehaviourTreeData too, probably)
	BehaviourTreeData m_Tree;
	BehaviourTreeState m_State;

	// TODO: figure out a better way to do builder pattern without using friend?
	friend class BehaviourTreeBuilder;
};

