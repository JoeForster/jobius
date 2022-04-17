#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>

void MockBehaviour::OnInitialise()
{
	m_TestCounter = 3;
	m_Status = BehaviourStatus::RUNNING;
}

BehaviourStatus MockBehaviour::Update()
{
	// TODO improve by using strategy pattern if it gets complex enough, otherwise just take a lambda?
	switch (m_Rule)
	{
		case MockRule::RUN_AND_SUCCEED:
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
		case MockRule::ALWAYS_FAIL:
		{
			m_Status = BehaviourStatus::FAILURE;
			break;
		}
	}
	return m_Status;
}

void MockBehaviour::OnTerminate(BehaviourStatus)
{
	m_TestCounter = -1;
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



// Debug out

using namespace std;

ostream& BehaviourTree::DebugToStream(ostream& stream) const
{
	stream << "BehaviourTree ->" << endl;
	int indentLevel = 1; // TODO
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
	// TODO reflection type name and status as string
	// TODO output children if not leaf
	stream << "Behaviour[Status:" << StatusString[(int)m_Status] << "]";
	return stream;
}

ostream& MockBehaviour::DebugToStream(ostream& stream) const
{
	stream << "MockBehaviour[Status:" << StatusString[(int)m_Status] << ", TestCounter:"<<m_TestCounter<<"]";
	return stream;
}

ostream& ActiveSelector::DebugToStream(ostream& stream) const
{
	const std::ptrdiff_t index = m_CurrentChild - m_Children.begin();
	stream << "ActiveSelector[Status:" << StatusString[(int)m_Status] << ", CurrentChild:" << index<<"/" << m_Children.size() << "]";
	//for (const auto& child : m_Children)
	//{
	//	stream << endl << "    ";
	//	child->DebugToStream(stream);
	//}
	return stream;
}
