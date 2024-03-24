#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>

// BehaviourTreeData

BehaviourTreeData::~BehaviourTreeData() 
{
	for (auto b : m_Behaviours)
	{
		delete b;
	}
}

// Behaviour

Behaviour::Behaviour(Behaviour* parent, BehaviourTreeData& treeData)
: m_Status(BehaviourStatus::INVALID)
, m_Parent(parent)
, m_TreeData(treeData)
{}

BehaviourStatus Behaviour::Tick(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
{
	if (m_Status != BehaviourStatus::RUNNING)
	{
		OnInitialise();
	}
	
	treeState.LastActiveNode = this;
	m_Status = Update(treeState, blackboard);

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

// BehaviourTreeHelpers

BehaviourStatus BehaviourTreeHelpers::TickTree(NPCBlackboardComponent& blackboard, BehaviourTreeData& treeData, BehaviourTreeState& treeState)
{
	assert(treeData.IsStructureLocked && "Behaviour tree ticked but not yet locked");
	if (!treeState.IsStarted)
	{
		// TODO: This is just temporary to verify a unit test. We want our own assertion/error-handling types
		// which are handled appropriately depending on whether we're running in engine, test mode, etc
		throw std::exception("Behaviour tree ticked but not yet started");
	}
	return treeData.m_Root->Tick(treeState, blackboard);
}

void BehaviourTreeHelpers::StepTree()
{
	assert(false && "Not implemented");
}

void BehaviourTreeHelpers::StartTree(BehaviourTreeState& state)
{
	assert(!state.IsStarted);
	state.IsStarted = true;
}

// Debug out

using namespace std;

ostream& BehaviourTreeHelpers::DebugOutTree(const Behaviour* root, ostream& stream)
{
	stream << "BehaviourTree[status=" << StatusString[(int)root->GetStatus()] << "]" << endl;
	int indentLevel = 1;
	const Behaviour* prevParent = nullptr;
	std::list<const Behaviour*> writeQueue = { root };
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
