#include "BehaviourTree.h"

#include <assert.h>
#include <algorithm>
#include <list>

#include "catch.hpp"

// TODO there's probably a more modern C++ way of getting an enum-string mapping like this
constexpr char* StatusString[] = {
	"INVALID",
	"RUNNING",
	"SUSPENDED",
	"SUCCESS",
	"FAILURE",
	"ABORTED",
};
static_assert(std::size(StatusString) == (size_t)BStatus::BSTATUS_COUNT);


void MockBehaviour::OnInitialise()
{
	m_TestCounter = 3;
	m_Status = BStatus::RUNNING;
}

BStatus MockBehaviour::Update()
{
	// TODO improve by using strategy pattern if it gets complex enough, otherwise just take a lambda?
	switch (m_Rule)
	{
		case MockRule::RUN_AND_SUCCEED:
		{

			if (--m_TestCounter <= 0)
			{
				m_Status = BStatus::SUCCESS;
			}
			else
			{
				m_Status = BStatus::RUNNING;
			}
			break;
		}
		case MockRule::ALWAYS_FAIL:
		{
			m_Status = BStatus::FAILURE;
			break;
		}
	}
	return m_Status;
}

void MockBehaviour::OnTerminate(BStatus)
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


// Unit Tests
// TODO more!

TEST_CASE("Build single-node tree", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.EndNode()
		.EndTree();
	REQUIRE(bt != nullptr);
}

TEST_CASE("Active selector test tree", "[BehaviourTree]")
{
	BehaviourTree* tree = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.AddNode_Mock(MockRule::ALWAYS_FAIL).EndNode()
			.AddNode_Mock(MockRule::RUN_AND_SUCCEED).EndNode()
			.EndNode()
		.EndTree();

	const ActiveSelector* root = dynamic_cast<const ActiveSelector*>( tree->GetRoot() );
	REQUIRE(root != nullptr);
	REQUIRE(root->GetChildCount() == 2);
	const MockBehaviour* failer = dynamic_cast<const MockBehaviour*>( root->GetChildAt(0) );
	REQUIRE(failer != nullptr);
	const MockBehaviour* succeeder = dynamic_cast<const MockBehaviour*>( root->GetChildAt(1) );
	REQUIRE(succeeder != nullptr);

	// TODO: Validate state
	REQUIRE(tree != nullptr);

	BStatus status;
	auto tickAndPrint = [&]
	{
		status = tree->Tick();
		tree->DebugToStream(cout) << endl << "  TREE STATUS --> " << StatusString[(int)status] << endl;

	};

	tickAndPrint();
	REQUIRE(status == BStatus::RUNNING);
	REQUIRE(failer->GetStatus() == BStatus::ABORTED); // TODO double-check this is valid, or should it be failure?
	REQUIRE(succeeder->GetStatus() == BStatus::RUNNING);

	tickAndPrint();
	REQUIRE(status == BStatus::RUNNING);
	REQUIRE(failer->GetStatus() == BStatus::FAILURE);
	REQUIRE(succeeder->GetStatus() == BStatus::RUNNING);

	tickAndPrint();
	REQUIRE(status == BStatus::SUCCESS);
	REQUIRE(failer->GetStatus() == BStatus::FAILURE);
	REQUIRE(succeeder->GetStatus() == BStatus::SUCCESS);

}

