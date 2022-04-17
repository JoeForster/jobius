#pragma once

#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

// First attempt at a very very naive and unoptimised behaviour tree implementation.
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
constexpr char* StatusString[] = {
	"INVALID",
	"RUNNING",
	"SUSPENDED",
	"SUCCESS",
	"FAILURE",
	"ABORTED",
};
static_assert(std::size(StatusString) == (size_t)BehaviourStatus::BSTATUS_COUNT);

class Behaviour
{
public:
	Behaviour(Behaviour* parent): m_Status(BehaviourStatus::INVALID), m_Parent(parent) {}
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

	BehaviourStatus m_Status;
	Behaviour* m_Parent; // TODO use smart pointers?
};

enum class MockRule
{
	RUN_AND_SUCCEED,
	ALWAYS_FAIL
};

class MockBehaviour : public Behaviour
{
public:
	MockBehaviour(Behaviour* parent, MockRule rule): Behaviour(parent), m_Rule(rule) {}

protected:
	void OnInitialise() override;
	BehaviourStatus Update() override;
	void OnTerminate(BehaviourStatus) override;

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

private:
	int m_TestCounter = -1;
	MockRule m_Rule;
};

class Decorator : public Behaviour
{
public:
	Decorator(Behaviour* parent, Behaviour& child): Behaviour(parent), m_Child(child) {}

	size_t GetChildCount() const final { return 1; }
	const Behaviour* GetChildAt(size_t index) const final { return (index == 0 ? &m_Child : nullptr); }

protected:
	Behaviour& m_Child;
};

// Repeat decorator: call the child N times immediately, if it succeeds.
class Repeat : public Decorator
{
public:
	Repeat(Behaviour* parent, Behaviour& child, unsigned numRepeats)
	: Decorator(parent, child)
	, m_NumRepeats(numRepeats)
	{}

protected:
	BehaviourStatus Repeat::Update() override;

private:
	unsigned m_RepeatCounter = 0;
	unsigned m_NumRepeats;
};


class Composite : public Behaviour
{
public:
	Composite(Behaviour* parent): Behaviour(parent) {}

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
	Sequence(Behaviour* parent): Composite(parent) {}

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update() override;

private:
	Behaviours::iterator m_CurrentChild;
};

class Filter : public Sequence
{
public:
	void AddCondition(Behaviour* condition);
	void AddAction(Behaviour* action);
};

class Parallel : public Composite
{
public:
	enum class Policy
	{
		RequireOne,
		RequireAll
	};

	Parallel(Behaviour* parent, Policy success, Policy failure)
	: Composite(parent)
	, m_SuccessPolicy(success)
	, m_FailurePolicy(failure)
	{
	}

protected:
	Policy m_SuccessPolicy;
	Policy m_FailurePolicy;

	virtual BehaviourStatus Update() override;
};

// Base "passive" selector: tick children until one succeeds or runs.
class Selector : public Composite
{
public:
	Selector(Behaviour* parent): Composite(parent) {}

protected:
	virtual void OnInitialise() override;
	virtual BehaviourStatus Update() override;

protected:
	Behaviours::iterator m_CurrentChild;
};

class Monitor : public Parallel
{
public:
	Monitor(Behaviour* parent): Parallel(parent, Policy::RequireOne, Policy::RequireOne) {}

	// TODO
};

// Active selector: like a passive selector but keeps re-checking higher-priority (leftmost) children before moving on to the next.
class ActiveSelector : public Selector
{
public:
	ActiveSelector(Behaviour* parent): Selector(parent) {}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

protected:
	BehaviourStatus Update() final;
};

class BehaviourTree
{
public:
	~BehaviourTree()
	{
		for (auto b : m_Behaviours)
		{
			delete b;
		}
	}

	BehaviourStatus Tick();
	void Step();
	void Start();

	const Behaviour* GetRoot() const { return m_Root; }

	virtual std::ostream& DebugToStream(std::ostream& stream) const;

private:
	Behaviour* m_Root = nullptr;
	std::vector<Behaviour*> m_Behaviours;

	// TODO: figure out a better way to do builder pattern without using friend?
	friend class BehaviourTreeBuilder;
};

