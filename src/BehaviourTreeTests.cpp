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
		tree->DebugToStream(std::cout) << std::endl << "  TREE STATUS --> " << StatusString[(int)status] << std::endl;

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

