#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>

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

BehaviourStatus Repeat::Update()
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

void Sequence::OnInitialise()
{
	// TODO: This won't work as expected if you alter the child set during run.
	// Does it ever make sense to restructure the tree during a run, or do we want just some flow to lock it after construction?
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
	m_Children.insert(m_Children.begin(), condition);
}
void Filter::AddAction(Behaviour* action)
{
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


void Monitor::AddCondition(Behaviour* condition)
{
	m_Children.insert(m_Children.begin(), condition);
}

void Monitor::AddAction(Behaviour* action)
{
	m_Children.push_back(action);
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

void MockAction::OnInitialise()
{
	m_TestCounter = 3;
	m_Status = BehaviourStatus::RUNNING;
}


void Composite::AddChild(Behaviour* behaviour)
{
	auto existing = std::find(m_Children.begin(), m_Children.end(), behaviour);
	assert(existing == m_Children.end());
	m_Children.push_back(behaviour);
}

void Composite::RemoveChild(Behaviour* behaviour)
{
	auto existing = std::find(m_Children.begin(), m_Children.end(), behaviour);
	assert(existing != m_Children.end());
	m_Children.erase(existing);
}

void Composite::ClearChildren()
{
	m_Children.clear();
}

BehaviourStatus BehaviourTree::Tick()
{
	return m_Root->Tick();
}

void BehaviourTree::Step()
{

}

void BehaviourTree::Start()
{

}

// Mocks

BehaviourStatus MockAction::Update()
{
	// TODO improve by using strategy pattern if it gets complex enough, otherwise just take a lambda?
	switch (m_Rule)
	{
	case MockActionRule::RUN_AND_SUCCEED:
	{

		if (--m_TestCounter <= 0)
		{
			m_Status = BehaviourStatus::SUCCESS;
		}
		else
		{
			m_Status = BehaviourStatus::RUNNING;
		}
		break;
	}
	case MockActionRule::ALWAYS_FAIL:
	{
		m_Status = BehaviourStatus::FAILURE;
		break;
	}
	}
	return m_Status;
}

void MockAction::OnTerminate(BehaviourStatus)
{
	m_TestCounter = -1;
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

ostream& MockAction::DebugToStream(ostream& stream) const
{
	stream << "MockAction[Status:" << StatusString[(int)m_Status] << ", TestCounter:"<<m_TestCounter<<"]";
	return stream;
}

ostream& ActiveSelector::DebugToStream(ostream& stream) const
{
	const std::ptrdiff_t index = m_CurrentChild - m_Children.begin();
	stream << "ActiveSelector[Status:" << StatusString[(int)m_Status] << ", CurrentChild:" << index<<"/" << m_Children.size() << "]";
	return stream;
}
