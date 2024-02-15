#pragma once

#include <vector>
#include <iostream>
#include <assert.h>

#include "BehaviourStatus.h"

// First attempt at a very very naive and unoptimised (first-generation, OOP) behaviour tree implementation.
// TODO Naming pass required (namespace, smurf, BStatus etc)

// Holder for state of behaviour tree accessible to nodes within
// (to prevent circular behaviour <--> tree dependency)
// May become an enum?
// TODO: some of this should be made const again, if we don't do away with this soon..
class Behaviour;

struct BehaviourTreeState
{
	bool IsStructureLocked = false;
	bool IsStarted = false;
	const Behaviour* LastActiveNode = nullptr;
};

class Behaviour
{
public:
	Behaviour(Behaviour* parent, BehaviourTreeState& treeState);
	virtual ~Behaviour() {}

	BehaviourStatus Tick();
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
	virtual BehaviourStatus Update() = 0;
	virtual void OnTerminate(BehaviourStatus) {}

	inline bool CanChangeStructure() const { return !m_TreeState.IsStructureLocked; }

	BehaviourStatus m_Status;
	Behaviour* m_Parent; // TODO use smart pointers?

private:
	BehaviourTreeState& m_TreeState;
};

// BEHAVIOUR TREE wrapper class
// - Owns the Behaviour memory once added
// - Tracks the state of the tree (locked/started)

class BehaviourTree
{
public:
	BehaviourTree() = default;
	~BehaviourTree();

	BehaviourStatus Tick();
	void Step();
	void Start();

	inline Behaviour* GetRoot() { return m_Root; }
	inline BehaviourTreeState& GetState() { return m_State; }

	virtual std::ostream& DebugToStream(std::ostream& stream) const;

private:
	Behaviour* m_Root = nullptr;
	std::vector<Behaviour*> m_Behaviours;

	BehaviourTreeState m_State;

	// TODO: figure out a better way to do builder pattern without using friend?
	friend class BehaviourTreeBuilder;
};

