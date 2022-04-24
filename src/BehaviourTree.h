#pragma once

#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

// First attempt at a very very naive and unoptimised (first-generation, OOP) behaviour tree implementation.
// TODO Naming pass required (namespace, smurf, BStatus etc)

enum class BehaviourStatus
{
	INVALID,
	RUNNING,
	SUSPENDED,
	SUCCESS,
	FAILURE,
	ABORTED,

	BSTATUS_COUNT
};

// TODO there's probably a more modern C++ way of getting an enum-string mapping like this
constexpr const char* StatusString[] = {
	"INVALID",
	"RUNNING",
	"SUSPENDED",
	"SUCCESS",
	"FAILURE",
	"ABORTED",
};
static_assert(std::size(StatusString) == (size_t)BehaviourStatus::BSTATUS_COUNT);

// Holder for state of behaviour tree accessible to nodes within
// (to prevent circular behaviour <--> tree dependency)
// May become an enum?
struct BehaviourTreeState
{
	bool IsStructureLocked = false;
	bool IsStarted = false;
};

class Behaviour
{
public:
	Behaviour(Behaviour* parent, const BehaviourTreeState& treeState);
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
	const BehaviourTreeState& m_TreeState;
};

class Decorator : public Behaviour
{
public:
	Decorator(Behaviour* parent, const BehaviourTreeState& treeState)
	: Behaviour(parent, treeState)
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
	Repeat(Behaviour* parent, const BehaviourTreeState& treeState, unsigned numRepeats)
	: Decorator(parent, treeState)
	, m_NumRepeats(numRepeats)
	{}

protected:
	BehaviourStatus Update() override;

private:
	unsigned m_RepeatCounter = 0;
	unsigned m_NumRepeats;
};


class Composite : public Behaviour
{
public:
	Composite(Behaviour* parent, const BehaviourTreeState& treeState): Behaviour(parent, treeState) {}

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
	Sequence(Behaviour* parent, const BehaviourTreeState& treeState): Composite(parent, treeState) {}

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update() override;

private:
	Behaviours::iterator m_CurrentChild;
};

class Filter : public Sequence
{
public:
	Filter(Behaviour* parent, const BehaviourTreeState& treeState): Sequence(parent, treeState) {}

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

	Parallel(Behaviour* parent, const BehaviourTreeState& treeState, Policy success, Policy failure)
	: Composite(parent, treeState)
	, m_SuccessPolicy(success)
	, m_FailurePolicy(failure)
	{
	}

protected:
	Policy m_SuccessPolicy;
	Policy m_FailurePolicy;

	virtual BehaviourStatus Update() override;
};

// Monitor node: a parallel node which will always re-check a set of conditions befor eexecuting the actions.
// Children sequence is a set of conditions first, then the actions.
class Monitor : public Parallel
{
public:
	Monitor(Behaviour* parent, const BehaviourTreeState& treeState)
	: Parallel(parent, treeState, Policy::RequireOne, Policy::RequireOne)
	{}

	void AddCondition(Behaviour* condition);
	void AddAction(Behaviour* action);

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;
};

// Base "passive" selector: tick children until one succeeds or runs.
class Selector : public Composite
{
public:
	Selector(Behaviour* parent, const BehaviourTreeState& treeState);

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update() override;

protected:
	Behaviours::iterator m_CurrentChild;
};

// Active selector: like a passive selector but keeps re-checking higher-priority (leftmost) children before moving on to the next.
class ActiveSelector : public Selector
{
public:
	ActiveSelector(Behaviour* parent, const BehaviourTreeState& treeState)
	: Selector(parent, treeState)
	{}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

protected:
	BehaviourStatus Update() final;
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
	inline const BehaviourTreeState& GetState() const { return m_State; }

	virtual std::ostream& DebugToStream(std::ostream& stream) const;

private:
	Behaviour* m_Root = nullptr;
	std::vector<Behaviour*> m_Behaviours;

	BehaviourTreeState m_State;

	// TODO: figure out a better way to do builder pattern without using friend?
	friend class BehaviourTreeBuilder;
};

