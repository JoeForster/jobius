#include "catch.hpp"

#include "BehaviourTree.h"
#include "BehaviourTreeBuilder.h"

// MOCK TYPES
// TODO likely to go into their own new header/cpp once this file gets too massive...
enum class MockActionRule
{
	SUCCEED,
	RUN_AND_SUCCEED,
	FAIL_AND_THEN_SUCCEED,
	ALWAYS_FAIL,

	MARULE_COUNT
};

constexpr const char* MockActionString[] = {
	"SUCCEED",
	"RUN_AND_SUCCEED",
	"FAIL_AND_THEN_SUCCEED",
	"ALWAYS_FAIL"
};
static_assert(std::size(MockActionString) == (size_t)MockActionRule::MARULE_COUNT);

class MockAction : public Behaviour
{
public:
	MockAction(Behaviour* parent, BehaviourTreeState& treeState, MockActionRule rule)
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
		case MockActionRule::SUCCEED:
		{
			m_Status = BehaviourStatus::SUCCESS;
			break;
		}
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
		case MockActionRule::FAIL_AND_THEN_SUCCEED:
		{

			if (--m_TestCounter <= 0)
			{
				m_Status = BehaviourStatus::SUCCESS;
			}
			else
			{
				m_Status = BehaviourStatus::FAILURE;
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

	virtual std::ostream& DebugToStream(std::ostream& stream) const override
	{
		stream << "MockAction[Status:" << StatusString[(int)m_Status] << ", TestCounter:"<<m_TestCounter<<"]";
		return stream;
	}

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
#
// Condition (test): run once, return result and terminate
// TODO: Could generalise to a Condition, perhaps taking a lambda or strategy?
class MockCondition : public Behaviour
{
public:
	MockCondition(Behaviour* parent, BehaviourTreeState& treeState, MockConditionRule rule)
		: Behaviour(parent, treeState), m_Rule(rule), m_TestCounter(1) {}

protected:
	void OnInitialise() override
	{
		// NOTE since basic conditions are "instantaneous" and then terminate,
		// We must persist our counter and not reset it here.
	}

	BehaviourStatus Update() override
	{
		switch (m_Rule)
		{
			case MockConditionRule::ALWAYS_SUCCEED:
				m_Status = BehaviourStatus::SUCCESS;
				break;
			case MockConditionRule::ALWAYS_FAIL:
				m_Status = BehaviourStatus::FAILURE;
				break;
			case MockConditionRule::FAIL_AND_THEN_SUCCEED:
				if (--m_TestCounter < 0)
				{
					m_Status = BehaviourStatus::SUCCESS;
					m_TestCounter = 0;
				}
				else
				{
					m_Status = BehaviourStatus::FAILURE;
				}
				break;
			case MockConditionRule::SUCEED_AND_THEN_FAIL:
				if (--m_TestCounter < 0)
				{
					m_Status = BehaviourStatus::FAILURE;
					m_TestCounter = 0;
				}
				else
				{
					m_Status = BehaviourStatus::SUCCESS;
				}
				break;
		}
		return m_Status;
	}

	void OnTerminate(BehaviourStatus) override
	{
	}

	virtual std::ostream& DebugToStream(std::ostream& stream) const override
	{
		stream << "MockCondition[Status:" << StatusString[(int)m_Status] << ", TestCounter:"<<m_TestCounter<<"]";
		return stream;
	}

private:
	int m_TestCounter = 1;
	MockConditionRule m_Rule;
};


// Unit Tests
// TODO common init/cleanup block (just make the builder, bt pointer, delete at end?)
// TODO more!
// TODO need to check against memory leaks too (but should be made redundant when we switch to custom allocator and/or smart pointers)

TEST_CASE("Build single-node tree", "[BehaviourTree]")
{
	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>().EndNode()
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
			.AddNode<Sequence>().EndNode()
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

// these are just for clarity in the unit test code right now.. But could grow?
// TOOD: more concise way of making these, without resorting to preprocessor macro?
class MockIsPlayerVisible : public MockCondition
{
public:
	MockIsPlayerVisible(Behaviour* parent, BehaviourTreeState& treeState)
	: MockCondition(parent, treeState, MockConditionRule::FAIL_AND_THEN_SUCCEED)
	{}
};

class MockIsPlayerInRange : public MockCondition
{
public:
	MockIsPlayerInRange(Behaviour* parent, BehaviourTreeState& treeState)
	: MockCondition(parent, treeState, MockConditionRule::FAIL_AND_THEN_SUCCEED)
	{}
};

class MockFireAtPlayer : public MockAction
{
public:
	MockFireAtPlayer(Behaviour* parent, BehaviourTreeState& treeState)
	: MockAction(parent, treeState, MockActionRule::SUCCEED)
	{}
};

class MockMoveToPlayersLKP : public MockAction
{
public:
	MockMoveToPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
	: MockAction(parent, treeState, MockActionRule::RUN_AND_SUCCEED)
	{}
};

class MockLookAround : public MockAction
{
public:
	MockLookAround(Behaviour* parent, BehaviourTreeState& treeState)
	: MockAction(parent, treeState, MockActionRule::RUN_AND_SUCCEED)
	{}
};

class MockCheckHasPlayersLKP : public MockCondition
{
public:
	MockCheckHasPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
	: MockCondition(parent, treeState, MockConditionRule::ALWAYS_SUCCEED)
	{}
};

class MockMoveToRandomPosition : public MockAction
{
public:
	MockMoveToRandomPosition(Behaviour* parent, BehaviourTreeState& treeState)
	: MockAction(parent, treeState, MockActionRule::RUN_AND_SUCCEED)
	{}
};

TEST_CASE("Simple NPC behaviour tree", "[BehaviourTree]")
{

	BehaviourTree* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>() // Root
			.AddNode<Sequence>() // Attack the player if seen
				.AddNode<MockIsPlayerVisible>().EndNode()
				.AddNode<ActiveSelector>()
					.AddNode<Sequence>()
						.AddNode<MockIsPlayerInRange>().EndNode()
						.AddNode<Repeat>(3)
							.AddNode<MockFireAtPlayer>().EndNode()
						.EndNode()
					.EndNode()
				.EndNode()
			.EndNode() // End sequence: Attack player if seen
			.AddNode<Sequence>() // Search near last-known position
				.AddNode<MockCheckHasPlayersLKP>().EndNode()
				.AddNode<MockMoveToPlayersLKP>().EndNode()
				.AddNode<MockLookAround>().EndNode()
			.EndNode() // End sequence: search near last-known position
			.AddNode<Sequence>() // Random wander
				.AddNode<MockMoveToRandomPosition>().EndNode()
				.AddNode<MockLookAround>().EndNode()
			.EndNode() // End sequence: random wander
		.EndNode() // End root active selector
		.EndTree();
	REQUIRE(bt != nullptr);

	bt->Start();

	BehaviourStatus status;
	auto tickAndPrint = [&]
	{
		status = bt->Tick();
		bt->DebugToStream(std::cout) << std::endl << "  TREE STATUS --> " << StatusString[(int)status] << std::endl;

	};
	
	// TODO: Update and validate some state
	tickAndPrint();

	delete bt;
}


