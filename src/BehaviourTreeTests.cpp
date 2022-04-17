#include "catch.hpp"

#include "BehaviourTree.h"
#include "BehaviourTreeBuilder.h"

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
			.AddNode_Mock(MockActionRule::ALWAYS_FAIL).EndNode()
			.AddNode_Mock(MockActionRule::RUN_AND_SUCCEED).EndNode()
			.EndNode()
		.EndTree();

	const ActiveSelector* root = dynamic_cast<const ActiveSelector*>( tree->GetRoot() );
	REQUIRE(root != nullptr);
	REQUIRE(root->GetChildCount() == 2);
	const MockAction* failer = dynamic_cast<const MockAction*>( root->GetChildAt(0) );
	REQUIRE(failer != nullptr);
	const MockAction* succeeder = dynamic_cast<const MockAction*>( root->GetChildAt(1) );
	REQUIRE(succeeder != nullptr);

	// TODO: Validate state
	REQUIRE(tree != nullptr);

	BehaviourStatus status;
	auto tickAndPrint = [&]
	{
		status = tree->Tick();
		tree->DebugToStream(std::cout) << std::endl << "  TREE STATUS --> " << StatusString[(int)status] << std::endl;

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

}

