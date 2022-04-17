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

	BehaviourStatus Tick()
	{
		if (m_Status != BehaviourStatus::RUNNING)
		{
			OnInitialise();
		}

		m_Status = Update();

		if (m_Status != BehaviourStatus::RUNNING)
		{
			OnTerminate(m_Status);
		}

		return m_Status;
	}

	void Abort()
	{
		OnTerminate(BehaviourStatus::ABORTED);
		m_Status = BehaviourStatus::ABORTED;
	}

	bool IsTerminated() const
	{
		return m_Status == BehaviourStatus::SUCCESS  ||  m_Status == BehaviourStatus::FAILURE;
	}

	bool IsRunning() const
	{
		return m_Status == BehaviourStatus::RUNNING;
	}

	BehaviourStatus GetStatus() const
	{
		return m_Status;
	}

	const Behaviour* GetParent() const
	{
		return m_Parent;
	}

	Behaviour* GetParent()
	{
		return m_Parent;
	}

	virtual size_t GetChildCount() const { return 0; }
	virtual const Behaviour* GetChildAt(size_t index) const { return nullptr; }

	// TEMP Construction - can be made unnecessary with templated/concept construction?
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
	BehaviourStatus Repeat::Update() override
	{
		while (true)
		{
			m_Child.Tick();

			const auto status = m_Child.GetStatus();
			if (status == BehaviourStatus::RUNNING)
			{
				break;
			}
			else if (status == BehaviourStatus::FAILURE)
			{
				return BehaviourStatus::FAILURE;
			}
			// TODO should deal with unexpected state value here
			else if (++m_RepeatCounter == m_NumRepeats)
			{
				return BehaviourStatus::SUCCESS;
			}
		}
		return BehaviourStatus::INVALID;
	}

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
	virtual void OnInitialise() override
	{
		// TODO: This won't work as expected if you alter the child set during run.
		// Does it ever make sense to restructure the tree during a run, or do we want just some flow to lock it after construction?
		m_CurrentChild = m_Children.begin();
	}

	virtual BehaviourStatus Update() override
	{
		assert(m_Children.size() > 0);

		// Run every child until one is running/fails/invalid or we reach the end.
		while (true)
		{
			BehaviourStatus childStatus = (*m_CurrentChild)->Tick();
			if (childStatus != BehaviourStatus::SUCCESS)
			{
				assert(childStatus != BehaviourStatus::INVALID);
				return childStatus;
			}
			if (++m_CurrentChild == m_Children.end())
			{
				return BehaviourStatus::SUCCESS;
			}
		}
		// TODO: handle case of no children?
		return BehaviourStatus::INVALID;
	}

private:
	Behaviours::iterator m_CurrentChild;
};

class Filter : public Sequence
{
public:
	void AddCondition(Behaviour* condition)
	{
		m_Children.insert(m_Children.begin(), condition);
	}
	void AddAction(Behaviour* action)
	{
		m_Children.push_back(action);
	}
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

	virtual BehaviourStatus Update() override
	{
		// TODO this doesn't deal with the case that the child list is empty!
		// TODO what about the case that all children are in the "invalid" state?

		size_t successCount = 0, failureCount = 0;
		assert(m_Children.size() > 0);
		for (auto child: m_Children)
		{
			if (!child->IsTerminated())
			{
				child->Tick();
			}
			const auto status = child->GetStatus();
			if (status == BehaviourStatus::SUCCESS)
			{
				++successCount;
				if (m_SuccessPolicy == Policy::RequireOne)
				{
					return BehaviourStatus::SUCCESS;
				}
			}
			if (status == BehaviourStatus::FAILURE)
			{
				++failureCount;
				if (m_FailurePolicy == Policy::RequireOne)
				{
					return BehaviourStatus::FAILURE;
				}
			}
		}
		if (m_FailurePolicy == Policy::RequireAll && failureCount == m_Children.size())
			return BehaviourStatus::FAILURE;
		if (m_FailurePolicy == Policy::RequireAll && failureCount == m_Children.size())
			return BehaviourStatus::FAILURE;

		return BehaviourStatus::RUNNING;
	}
};

// Base "passive" selector: tick children until one succeeds or runs.
class Selector : public Composite
{
public:
	Selector(Behaviour* parent): Composite(parent) {}

protected:
	virtual void OnInitialise() override
	{
		// TODO: This won't work as expected if you alter the child set during run.
		// Does it ever make sense to restructure the tree during a run, or do we want just some flow to lock it after construction?
		m_CurrentChild = m_Children.begin();
	}

	virtual BehaviourStatus Update() override
	{
		assert(m_Children.size() > 0);

		// Tick every child until one succeeds/runs - or fail if none do so.
		while (true)
		{
			BehaviourStatus childStatus = (*m_CurrentChild)->Tick();
			// Child either ran successfully or is in-progress
			if (childStatus != BehaviourStatus::FAILURE)
			{
				//assert(childStatus != BehaviourStatus::INVALID);
				return childStatus;
			}
			// If we ran out, then the whole selector failed to select
			if (++m_CurrentChild == m_Children.end())
			{
				return BehaviourStatus::FAILURE;
			}
		}
		// TODO: handle case of no children?
		return BehaviourStatus::INVALID;
	}

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
	BehaviourStatus Update() final
	{
		Behaviours::iterator prev = m_CurrentChild;
		OnInitialise();
		BehaviourStatus result = Selector::Update();
		if (prev != m_Children.end() && m_CurrentChild != prev)
		{
			(*prev)->Abort();
		}
		return result;
	}
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

