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

void MockBehaviour::OnShutdown(BStatus)
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

void ActiveSelector::OnInitialise()
{

}

BStatus ActiveSelector::Update()
{
	// TODO
	return BStatus::INVALID;
}

void ActiveSelector::OnShutdown(BStatus)
{

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
	auto& btb = BehaviourTreeBuilder();
	btb = btb.AddNode_ActiveSelector();
	btb = btb.EndNode();

	//BehaviourTree* bt = BehaviourTreeBuilder().AddNode_ActiveSelector().End();
	BehaviourTree* bt = btb.EndTree();
	REQUIRE(bt != nullptr);
}
