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

void BehaviourTree::Tick()
{

}

void BehaviourTree::Step()
{

}

void BehaviourTree::Start()
{

}

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
	// TODO tick and validate

}

