#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>

#include "catch.hpp"


void MockBehaviour::OnInitialise()
{
	m_TestCounter = 3;
	m_Status = BStatus::RUNNING;
}

BStatus MockBehaviour::Update()
{
	if (--m_TestCounter <= 0)
	{
		m_Status = BStatus::SUCCESS;
	}
	else
	{
		m_Status = BStatus::RUNNING;
	}
	return m_Status;
}

void MockBehaviour::OnTerminate(BStatus)
{
	m_TestCounter = -1;
	m_Status = BStatus::INVALID;
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

BStatus BehaviourTree::Tick()
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

ostream& BehaviourTree::DebugToStream(ostream& stream)
{
	m_Root->DebugToStream(stream);
	return stream;
}

ostream& Behaviour::DebugToStream(ostream& stream)
{
	// TODO reflection type name and status as string
	// TODO output children if not leaf
	stream << "Behaviour[Status:" << (int)m_Status << "]";
	return stream;
}

ostream& MockBehaviour::DebugToStream(ostream& stream)
{
	stream << "MockBehaviour[Status:" << (int)m_Status << ", TestCounter:"<<m_TestCounter<<"]";
	return stream;
}

ostream& ActiveSelector::DebugToStream(ostream& stream)
{
	const std::ptrdiff_t index = m_CurrentChild - m_Children.begin();
	stream << "ActiveSelector[Status:" << (int)m_Status << ", CurrentChild:" << index<<"/" << m_Children.size() << "]";
	for (const auto& child : m_Children)
	{
		stream << endl << "    ";
		child->DebugToStream(stream);
	}
	return stream;
}


// Unit Tests

TEST_CASE("Build single-node tree", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.EndNode()
		.EndTree();
	REQUIRE(bt != nullptr);
	// TODO tick and validate
}

TEST_CASE("Active selector test tree", "[BehaviourTree]")
{
	BehaviourTree* tree = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.AddNode<MockBehaviour>().EndNode()
			.AddNode<MockBehaviour>().EndNode()
			.EndNode()
		.EndTree();

	// TODO: Validate state
	REQUIRE(tree != nullptr);
	tree->Tick();
	tree->DebugToStream(cout) << endl;
	tree->Tick();
	tree->DebugToStream(cout) << endl;
	tree->Tick();
	tree->DebugToStream(cout) << endl;
	tree->Tick();
	tree->DebugToStream(cout) << endl;
	tree->Tick();
	tree->DebugToStream(cout) << endl;
	tree->Tick();


	// TODO validate state

}

