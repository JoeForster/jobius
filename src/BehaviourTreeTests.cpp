#include "catch.hpp"

#include "BehaviourTree.h"
#include "BehaviourTreeBuilder.h"

// MOCK TYPES
// TODO likely to go into their own new header/cpp once this file gets too massive...
enum class MockActionRule
{
	RUN_AND_SUCCEED,
	ALWAYS_FAIL,

	MARULE_COUNT
};

constexpr const char* MockActionString[] = {
	"RUN_AND_SUCCEED",
	"ALWAYS_FAIL"
};
static_assert(std::size(MockActionString) == (size_t)MockActionRule::MARULE_COUNT);

class MockAction : public Behaviour
{
public:
	MockAction(Behaviour* parent, const BehaviourTreeState& treeState, MockActionRule rule)
	: Behaviour(parent, treeState), m_Rule(rule) {}

protected:
	void OnInitialise() override
	{
		m_TestCounter = 3;
		m_Status = BehaviourStatus::RUNNING;
	}

	BehaviourStatus Update()
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

	void OnTerminate(BehaviourStatus) override
	{
		m_TestCounter = -1;
	}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

private:
	int m_TestCounter = -1;
	MockActionRule m_Rule;
};

enum class MockConditionRule
{
	ALWAYS_SUCCEED,
	ALWAYS_FAIL,
	FAIL_AND_THEN_SUCCEED,
	SUCEED_AND_THEN_FAIL,

	MCRULE_COUNT
};

constexpr const char* MockConditionString[] = {
	"ALWAYS_SUCCEED",
	"ALWAYS_FAIL",
	"FAIL_AND_THEN_SUCCEED",
	"SUCEED_AND_THEN_FAIL"
};
static_assert(std::size(MockConditionString) == (size_t)MockConditionRule::MCRULE_COUNT);

class MockCondition : public Behaviour
{
public:
	MockCondition(Behaviour* parent, const BehaviourTreeState& treeState, MockConditionRule rule)
		: Behaviour(parent, treeState), m_Rule(rule) {}

protected:
	void OnInitialise() override;
	BehaviourStatus Update() override;
	void OnTerminate(BehaviourStatus) override;

	virtual std::ostream& DebugToStream(std::ostream& stream) const override;

private:
	MockConditionRule m_Rule;
};

std::ostream& MockAction::DebugToStream(std::ostream& stream) const
{
	stream << "MockAction[Status:" << StatusString[(int)m_Status] << ", TestCounter:"<<m_TestCounter<<"]";
	return stream;
}


// Unit Tests
// TODO common init/cleanup block (just make the builder, bt pointer, delete at end?)
// TODO more!
// TODO need to check against memory leaks too (but should be made redundant when we switch to custom allocator and/or smart pointers)

TEST_CASE("Build single-node tree", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.EndNode()
		.EndTree();
	REQUIRE(bt != nullptr);

	delete bt;
}

TEST_CASE("Illegal tree update", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.AddNode<MockAction>(MockActionRule::ALWAYS_FAIL).EndNode()
			.AddNode<MockAction>(MockActionRule::RUN_AND_SUCCEED).EndNode()
			.EndNode()
		.EndTree();
		
	// Tick a tree before starting it
	REQUIRE_THROWS_WITH(bt->Tick(), "Behaviour tree ticked but not yet started");

	delete bt;

}

TEST_CASE("Illegal modification of structure", "[BehaviourTree]")
{
	auto builder = BehaviourTreeBuilder();
	BehaviourTree* bt = builder
			.AddNode<Sequence>()
			.EndNode()
		.EndTree();
	REQUIRE(bt != nullptr);

	Composite* root = dynamic_cast<Composite*>( bt->GetRoot() );
	REQUIRE(root != nullptr);
	REQUIRE(root->GetChildCount() == 0);

	// TODO: Rethink needed maybe: bit of a hacky test which bypasses the normal BehaviourTreeBuilder interface,
	// since EndTree clears the parent node that would be used and asserts.
	// Want to do something like:
	//REQUIRE_THROWS_WITH(builder.AddNode<MockAction>(MockActionRule::ALWAYS_FAIL), "ILLEGAL change of tree structure!");
	// But does it make sense to mess with the builder interface just for this test?
	// Instead for now:
	Behaviour* bh = new MockAction(root, bt->GetState(), MockActionRule::ALWAYS_FAIL);
	REQUIRE_THROWS_WITH(root->AddChild(bh), "ILLEGAL change of tree structure!");
	delete bh; // Should have failed to add so tree didn't take ownership of this memory

	delete bt;

}

TEST_CASE("Active selector test tree", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>()
			.AddNode<MockAction>(MockActionRule::ALWAYS_FAIL).EndNode()
			.AddNode<MockAction>(MockActionRule::RUN_AND_SUCCEED).EndNode()
			.EndNode()
		.EndTree();

	const ActiveSelector* root = dynamic_cast<const ActiveSelector*>( bt->GetRoot() );
	REQUIRE(root != nullptr);
	REQUIRE(root->GetChildCount() == 2);
	const MockAction* failer = dynamic_cast<const MockAction*>( root->GetChildAt(0) );
	REQUIRE(failer != nullptr);
	const MockAction* succeeder = dynamic_cast<const MockAction*>( root->GetChildAt(1) );
	REQUIRE(succeeder != nullptr);

	// TODO: Validate state
	REQUIRE(bt != nullptr);

	bt->Start();

	BehaviourStatus status;
	auto tickAndPrint = [&]
	{
		status = bt->Tick();
		bt->DebugToStream(std::cout) << std::endl << "  TREE STATUS --> " << StatusString[(int)status] << std::endl;

	};

	tickAndPrint();
	REQUIRE(status == BehaviourStatus::RUNNING);
	REQUIRE(failer->GetStatus() == BehaviourStatus::ABORTED); // TODO double-check this is valid, or should it be failure?
	REQUIRE(succeeder->GetStatus() == BehaviourStatus::RUNNING);

	tickAndPrint();
	REQUIRE(status == BehaviourStatus::RUNNING);
	REQUIRE(failer->GetStatus() == BehaviourStatus::FAILURE);
	REQUIRE(succeeder->GetStatus() == BehaviourStatus::RUNNING);

	tickAndPrint();
	REQUIRE(status == BehaviourStatus::SUCCESS);
	REQUIRE(failer->GetStatus() == BehaviourStatus::FAILURE);
	REQUIRE(succeeder->GetStatus() == BehaviourStatus::SUCCESS);

	delete bt;
}

