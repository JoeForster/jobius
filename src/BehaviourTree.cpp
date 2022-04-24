#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>


Behaviour::Behaviour(Behaviour* parent, const BehaviourTreeState& treeState)
: m_Status(BehaviourStatus::INVALID)
, m_Parent(parent)
, m_TreeState(treeState)
{}

BehaviourStatus Behaviour::Tick()
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

void Behaviour::Abort()
{
	OnTerminate(BehaviourStatus::ABORTED);
	m_Status = BehaviourStatus::ABORTED;
}

bool Behaviour::IsTerminated() const
{
	return m_Status == BehaviourStatus::SUCCESS  ||  m_Status == BehaviourStatus::FAILURE;
}

bool Behaviour::IsRunning() const
{
	return m_Status == BehaviourStatus::RUNNING;
}

void Decorator::OnInitialise()
{
	assert(m_Child != nullptr && "Decorator OnInitialise no child set");
}

BehaviourStatus Repeat::Update()
{
	assert(m_Child != nullptr);
	while (true)
	{
		m_Child->Tick();

		const auto status = m_Child->GetStatus();
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

void Sequence::OnInitialise()
{
	// TODO_ERRORHANDLING This won't work as expected if you alter the child set during run.
	// For now we use the CanChangeStructure check to guard against this.
	m_CurrentChild = m_Children.begin();
}

BehaviourStatus Sequence::Update()
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


void Filter::AddCondition(Behaviour* condition)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	m_Children.insert(m_Children.begin(), condition);
}
void Filter::AddAction(Behaviour* action)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	m_Children.push_back(action);
}


BehaviourStatus Parallel::Update()
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


void Monitor::AddCondition(Behaviour* condition)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	m_Children.insert(m_Children.begin(), condition);
}

void Monitor::AddAction(Behaviour* action)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	m_Children.push_back(action);
}


void Selector::OnInitialise()
{
	// TODO: This won't work as expected if you alter the child set during run.
	// Does it ever make sense to restructure the tree during a run, or do we want just some flow to lock it after construction?
	m_CurrentChild = m_Children.begin();
}

BehaviourStatus Selector::Update()
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


BehaviourStatus ActiveSelector::Update()
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

void Composite::AddChild(Behaviour* behaviour)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	auto existing = std::find(m_Children.begin(), m_Children.end(), behaviour);
	assert(existing == m_Children.end());
	m_Children.push_back(behaviour);
}

void Composite::RemoveChild(Behaviour* behaviour)
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	auto existing = std::find(m_Children.begin(), m_Children.end(), behaviour);
	assert(existing != m_Children.end());
	m_Children.erase(existing);
}

void Composite::ClearChildren()
{
	if (!CanChangeStructure())
	{
		// TODO_ERRORHANDLING: temp just for unit tests
		throw std::exception("ILLEGAL change of tree structure!");
	}
	m_Children.clear();
}

BehaviourTree::~BehaviourTree()
{
	for (auto b : m_Behaviours)
	{
		delete b;
	}
}

BehaviourStatus BehaviourTree::Tick()
{
	assert(m_State.IsStructureLocked && "Behaviour tree ticked but not yet locked");
	if (!m_State.IsStarted)
	{
		// TODO: This is just temporary to verify a unit test. We want our own assertion/error-handling types
		// which are handled appropriately depending on whether we're running in engine, test mode, etc
		throw std::exception("Behaviour tree ticked but not yet started");
	}
	return m_Root->Tick();
}

void BehaviourTree::Step()
{
	assert(false && "Not implemented");
}

void BehaviourTree::Start()
{
	assert(!m_State.IsStarted);
	m_State.IsStarted = true;
}

// Debug out

using namespace std;

ostream& BehaviourTree::DebugToStream(ostream& stream) const
{
	stream << "BehaviourTree ->" << endl;
	int indentLevel = 1;
	const Behaviour* prevParent = nullptr;
	std::list<const Behaviour*> writeQueue = { m_Root };
	while (writeQueue.empty() == false)
	{
		const Behaviour* bh = writeQueue.front();
		writeQueue.pop_front();
		if (prevParent != bh->GetParent())
			indentLevel--;
		prevParent = bh->GetParent();

		for (int i = 0; i < indentLevel; ++i)
			stream << '\t';
		bh->DebugToStream(stream) << endl;

		const size_t num_children = bh->GetChildCount();
		if (num_children > 0)
		{
			size_t i = num_children;
			do
			{
				indentLevel++;
				const Behaviour* nextChild = bh->GetChildAt(--i);
				assert(nextChild != nullptr);
				writeQueue.emplace_front(nextChild);
			} while (i > 0);
		}
	}
	return stream;
}

ostream& Behaviour::DebugToStream(ostream& stream) const
{
	stream << "Behaviour[Status:" << StatusString[(int)m_Status] << "]";
	return stream;
}

ostream& ActiveSelector::DebugToStream(ostream& stream) const
{
	const std::ptrdiff_t index = m_CurrentChild - m_Children.begin();
	stream << "ActiveSelector[Status:" << StatusString[(int)m_Status] << ", CurrentChild:" << index<<"/" << m_Children.size() << "]";
	return stream;
}

ostream& Monitor::DebugToStream(ostream& stream) const
{
	stream << "Monitor[Status:" << StatusString[(int)m_Status] << "]";
	return stream;
}