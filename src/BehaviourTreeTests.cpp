#include "catch.hpp"

#include "BehaviourTree.h"
#include "BehaviourTreeBuilder.h"

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

	// NOTE: This is a bit of a hacky test which bypasses the normal BehaviourTreeBuilder interface. 
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

