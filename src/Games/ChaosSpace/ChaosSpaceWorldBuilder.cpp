#include "ChaosSpaceWorldBuilder.h"

#include "World.h"
#include "SDLRenderManager.h"
#include "BehaviourTreeBuilder.h"
#include "Behaviours.h"
#include "TestBehaviourNodes.h"

#include "BoxCollisionSystem.h"
#include "NPCControlSystem.h"
#include "NPCSensorSystem.h"
#include "PhysicsSystem.h"
#include "PlaneCollisionSystem.h"
#include "PlayerControlSystem.h"
#include "SDLInputSystem.h"
#include "Camera2DSystem.h"
#include "SpriteRenderSystem.h"

#include "AABBComponent.h"
#include "DebugTextComponent.h"
#include "GridTransformComponent.h"
#include "GridWorldComponent.h"
#include "KBInputComponent.h"
#include "NPCBlackboardComponent.h"
#include "PadInputComponent.h"
#include "PlaneComponent.h"
#include "PlayerComponent.h"
#include "RigidBodyComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "Camera2DComponent.h"
#include "BehaviourTreeComponent.h"
#include "BehaviourNodeComponent.h"

std::shared_ptr<World> ChaosSpaceWorldBuilder::BuildWorld(std::shared_ptr<SDLRenderManager> renderMan)
{
	std::shared_ptr<World> world = std::make_shared<World>();

	ResourceID resID_asteroid = renderMan->LoadTexture("assets/sprites/asteroid.png");
	assert(resID_asteroid != ResourceID_Invalid);
	ResourceID resID_fighter = renderMan->LoadTexture("assets/sprites/fighter_lr.png");
	assert(resID_fighter != ResourceID_Invalid);
	ResourceID resID_ufo = renderMan->LoadTexture("assets/sprites/ufo.png");
	assert(resID_ufo != ResourceID_Invalid);


	// TODO Currently we get a crash if we add a component we forgot to register. Can we check at compile time?
	world->RegisterComponent<TransformComponent>();
	world->RegisterComponent<SpriteComponent>();
	world->RegisterComponent<RigidBodyComponent>();
	world->RegisterComponent<AABBComponent>();
	world->RegisterComponent<PlaneComponent>();
	world->RegisterComponent<KBInputComponent>();
	world->RegisterComponent<PadInputComponent>();
	world->RegisterComponent<NPCBlackboardComponent>();
	world->RegisterComponent<PlayerComponent>();
	world->RegisterComponent<DebugTextComponent>();
	world->RegisterComponent<GridWorldComponent>();
	world->RegisterComponent<Camera2DComponent>();
	world->RegisterComponent<BehaviourTreeComponent>();
	world->RegisterComponent<BehaviourNodeComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<Camera2DSystem>()->Init(renderInit);
	world->RegisterSystem<PlayerControlSystem>()->Init(renderInit);
	world->RegisterSystem<NPCControlSystem>()->Init();
	world->RegisterSystem<NPCSensorSystem>()->Init();
	world->RegisterSystem<PhysicsSystem>()->Init(renderInit);
	world->RegisterSystem<BoxCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<PlaneCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<SpriteRenderSystem>()->Init(renderInit);

	// Create GLOBAL components
	// TODO FIXME HACK worakround for bug when removing entity 0 since the global components are stored in the same place!
	EntityID globalHack = world->CreateEntity();
	assert(globalHack == 0);
	world->SetGlobalComponent<GridWorldComponent>( { Rect2f{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32 } );
	world->SetGlobalComponent<Camera2DComponent>();

	// Create test world entities
	auto createSprite = [](World& w, ResourceID resID, Vector3f pos)
	{
		EntityID e = w.CreateEntity();
		TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, std::move(t));
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createGridSprite = [](World& w, ResourceID resID, Vector2i pos)
	{
		EntityID e = w.CreateEntity();
		GridTransformComponent t(pos);
		w.AddComponent<GridTransformComponent>(e, std::move(t));
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createSpriteWithPhysics = [](World& w, ResourceID resID, Vector3f pos, Vector2f boxSize, Vector2f boxOffset)
	{
		EntityID e = w.CreateEntity();
		TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, std::move(t));
		w.AddComponent<SpriteComponent>(e, resID);
		w.AddComponent<RigidBodyComponent>(e);
		w.AddComponent<AABBComponent>(e, { boxSize, boxOffset } );
		w.AddComponent<DebugTextComponent>(e);
		return e;
	};
	int nextPlayerIndex = 0;
	auto createPlayer = [&](Vector3f pos)
	{
		auto playerEntity = createSpriteWithPhysics(*world, resID_fighter, pos, {120, 54}, {60, 27});
		world->AddComponent<KBInputComponent>(playerEntity);
		world->AddComponent<PadInputComponent>(playerEntity, {nextPlayerIndex});
		world->AddComponent<PlayerComponent>(playerEntity, {nextPlayerIndex});
		++nextPlayerIndex;
	};

	createSprite(*world, resID_asteroid, { 50, 0, 0 });
	createSpriteWithPhysics(*world, resID_asteroid, { 150, 0, 0 }, { 70, 60 }, { 35, 30 });

	createPlayer({ 500, 0, 0 });
	createPlayer({ 700, 0, 0 });

	// Create AI test entity
	auto ufoEntity = createSpriteWithPhysics(*world, resID_ufo, { 200, 200, 200 }, { 158, 48 }, { 79, 24 });
	world->AddComponent<NPCBlackboardComponent>(ufoEntity);

	BehaviourTreeInstance* bt = BehaviourTreeBuilder()
		.AddNode<ActiveSelector>() // Root
			.AddNode<Sequence>() // Attack the player if seen
				.AddNode<CheckPlayerVisible>().EndNode()
				.AddNode<ActiveSelector>()
					.AddNode<Sequence>()
						.AddNode<IsPlayerInRange>().EndNode()
						.AddNode<Repeat>(3)
							.AddNode<FireAtPlayer>().EndNode()
						.EndNode()
					.EndNode()
				.EndNode()
			.EndNode() // End sequence: Attack player if seen
			.AddNode<Sequence>() // Search near last-known position
				.AddNode<CheckHasPlayersLKP>().EndNode()
				.AddNode<MoveToPlayersLKP>().EndNode()
				.AddNode<LookAround>().EndNode()
			.EndNode() // End sequence: search near last-known position
			.AddNode<Sequence>() // Random wander
				.AddNode<MoveToRandomPosition>().EndNode()
				.AddNode<LookAround>().EndNode()
			.EndNode() // End sequence: random wander
		.EndNode() // End root active selector
	.EndTree();

	bt->Start();

	world->AddComponent<BehaviourTreeComponent>(ufoEntity, BehaviourTreeComponent{ bt });


	return world;
}
