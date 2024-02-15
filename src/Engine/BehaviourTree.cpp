#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>


Behaviour::Behaviour(Behaviour* parent, BehaviourTreeState& treeState)
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
	
	m_TreeState.LastActiveNode = this;
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
	stream << "BehaviourTree[status=" << StatusString[(int)m_Root->GetStatus()] << "]" << endl;
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
	stream << typeid(*this).name() << "[Status:" << StatusString[(int)m_Status] << "]";
	return stream;
}
