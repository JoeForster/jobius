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

ostream& operator<<(ostream& stream, const BehaviourTree& bt)
{
	const Behaviour& bh = *bt.m_Root;
	stream << bh;
	return stream;
}

ostream& operator<<(ostream& stream, const Behaviour& bh)
{
	// TODO reflection type name and status as string
	// TODO output children if not leaf
	stream << "Behaviour[Status:" << (int)bh.m_Status << "]";
	return stream;
}

ostream& operator<<(ostream& stream, const MockBehaviour& bh)
{
	stream << "MockBehaviour[Status:" << (int)bh.m_Status << ", TestCounter:"<<bh.m_TestCounter<<"]";
	return stream;
}

ostream& operator<<(ostream& stream, const ActiveSelector& bh)
{
	const std::ptrdiff_t index = bh.m_CurrentChild - bh.m_Children.begin();
	stream << "ActiveSelector[Status:" << (int)bh.m_Status << ", CurrentChild:" << index<<"/" << bh.m_Children.size() << "]";
	for (const auto& child : bh.m_Children)
		stream << endl << "    " << *child;
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

	REQUIRE(tree != nullptr);
	tree->Tick();
	cout << *tree << endl;


	// TODO validate state

}

