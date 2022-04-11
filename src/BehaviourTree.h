#pragma once

#include <vector>
#include <assert.h>

// First attempt at a very very naive and unoptimised behaviour tree implementation.

enum class BStatus
{
	INVALID,
	RUNNING,
	SUSPENDED,
	SUCCESS,
	FAILURE,
};

class Behaviour
{
public:
	Behaviour(Behaviour* parent): m_Status(BStatus::INVALID), m_Parent(parent) {}
	virtual ~Behaviour() {}

	BStatus Tick()
	{
		if (m_Status != BStatus::RUNNING)
		{
			OnInitialise();
		}

		m_Status = Update();

		if (m_Status != BStatus::RUNNING)
		{
			OnTerminate(m_Status);
		}

		return m_Status;
	}

	bool IsTerminated() const
	{
		return m_Status == BStatus::SUCCESS  ||  m_Status == BStatus::FAILURE;
	}

	bool IsRunning() const
	{
		return m_Status == BStatus::RUNNING;
	}

	BStatus GetStatus() const
	{
		return m_Status;
	}

	Behaviour* GetParent()
	{
		return m_Parent;
	}

protected:
	virtual void OnInitialise() {}
	virtual BStatus Update() = 0;
	virtual void OnTerminate(BStatus) {}

	BStatus m_Status;
	Behaviour* m_Parent; // TODO use smart pointers?
};

class MockBehaviour : public Behaviour
{
public:
	MockBehaviour(Behaviour* parent): Behaviour(parent) {}

protected:
	void OnInitialise() override;
	BStatus Update() override;
	void OnTerminate(BStatus) override;

private:
	int m_TestCounter = -1;
};

class Decorator : public Behaviour
{
public:
	Decorator(Behaviour* parent, Behaviour& child): Behaviour(parent), m_Child(child) {}

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
	BStatus Repeat::Update() override
	{
		while (true)
		{
			m_Child.Tick();

			const auto status = m_Child.GetStatus();
			if (status == BStatus::RUNNING)
			{
				break;
			}
			else if (status == BStatus::FAILURE)
			{
				return BStatus::FAILURE;
			}
			// TODO should deal with unexpected state value here
			else if (++m_RepeatCounter == m_NumRepeats)
			{
				return BStatus::SUCCESS;
			}
		}
		return BStatus::INVALID;
	}

private:
	unsigned m_RepeatCounter = 0;
	unsigned m_NumRepeats;
};


class Composite : public Behaviour
{
public:
	Composite(Behaviour* parent): Behaviour(parent) {}

	void AddChild(Behaviour*);
	void RemoveChild(Behaviour*);
	void ClearChildren();

protected:
	// TODO would a list or deque make more sense?
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

	virtual BStatus Update() override
	{
		assert(m_Children.size() > 0);

		// Run every child until one is running/fails/invalid or we reach the end.
		while (true)
		{
			BStatus childStatus = (*m_CurrentChild)->Tick();
			if (childStatus != BStatus::SUCCESS)
			{
				assert(childStatus != BStatus::INVALID);
				return childStatus;
			}
			if (++m_CurrentChild == m_Children.end())
			{
				return BStatus::SUCCESS;
			}
		}
		// TODO: handle case of no children?
		return BStatus::INVALID;
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

	virtual BStatus Update() override
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
			if (status == BStatus::SUCCESS)
			{
				++successCount;
				if (m_SuccessPolicy == Policy::RequireOne)
				{
					return BStatus::SUCCESS;
				}
			}
			if (status == BStatus::FAILURE)
			{
				++failureCount;
				if (m_FailurePolicy == Policy::RequireOne)
				{
					return BStatus::FAILURE;
				}
			}
		}
		if (m_FailurePolicy == Policy::RequireAll && failureCount == m_Children.size())
			return BStatus::FAILURE;
		if (m_FailurePolicy == Policy::RequireAll && failureCount == m_Children.size())
			return BStatus::FAILURE;

		return BStatus::RUNNING;
	}
};

// TODO
class Selector : public Composite
{
public:
	Selector(Behaviour* parent): Composite(parent) {}

	// TODO
};

class Monitor : public Parallel
{
public:
	Monitor(Behaviour* parent): Parallel(parent, Policy::RequireOne, Policy::RequireOne) {}

	// TODO
};

class ActiveSelector : public Parallel
{
public:
	ActiveSelector(Behaviour* parent): Parallel(parent, Policy::RequireOne, Policy::RequireOne) {}

protected:
	void OnInitialise() final;
	BStatus Update() final;
	void OnTerminate(BStatus) final;
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

	void Tick();
	void Step();
	void Start();


private:
	Behaviour* m_Root = nullptr;
	std::vector<Behaviour*> m_Behaviours;

	// TODO: figure out a better way to do builder pattern without using friend?
	friend class BehaviourTreeBuilder;
};

class BehaviourTreeBuilder
{
public:
	BehaviourTreeBuilder()
	: m_Tree(new BehaviourTree)
	, m_CurrentBehaviour(nullptr)
	{
	}

	// TODO: Template this (requires figuring out how to pass in construction params)
	BehaviourTreeBuilder& AddNode_ActiveSelector()
	{
		Behaviour* node = new ActiveSelector(m_CurrentBehaviour);
		return AddNode(node);
	}

	BehaviourTreeBuilder& EndNode()
	{
		if (m_CurrentBehaviour == m_Tree->m_Root)
		{
			// done - root should have no parent
			assert(m_CurrentBehaviour->GetParent() == nullptr);
			m_CurrentBehaviour = nullptr;
		}
		else
		{
			m_CurrentBehaviour = m_CurrentBehaviour->GetParent();
		}

		return *this;
	}

	BehaviourTree* EndTree()
	{
		// We should have gone back to the root via EndNode
		assert(m_CurrentBehaviour == nullptr);
		return m_Tree;
	}

private:
	BehaviourTree* m_Tree;
	Behaviour* m_CurrentBehaviour;

	// TODO would be nice, but how would we do this with variable constructor params?
	//template<class T>
	//BehaviourTreeBuilder& AddNode()
	// Add a newly-constructed node (takes ownership of memory)
	BehaviourTreeBuilder& AddNode(Behaviour* newNode)
	{
		Behaviour* nodeParent = newNode->GetParent();
		assert(newNode->GetParent() == m_CurrentBehaviour);
		if (m_Tree->m_Root == nullptr)
		{
			assert(nodeParent == nullptr);
			m_Tree->m_Root = newNode;
		}
		else
		{
			assert(nodeParent != nullptr);
		}

		m_Tree->m_Behaviours.push_back(newNode);

		m_CurrentBehaviour = newNode;

		return *this;
	}
};