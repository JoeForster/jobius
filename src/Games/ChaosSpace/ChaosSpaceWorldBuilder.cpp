#include "ChaosSpaceWorldBuilder.h"

// FIXME proper include paths from game builders

#include "../../Engine/World.h"

#include "../../Engine/BoxCollisionSystem.h"
#include "../../Engine/NPCControlSystem.h"
#include "../../Engine/NPCSensorSystem.h"
#include "../../Engine/PhysicsSystem.h"
#include "../../Engine/PlaneCollisionSystem.h"
#include "../../Engine/PlayerControlSystem.h"
#include "../../Engine/SDLInputSystem.h"
#include "../../Engine/SpriteRenderSystem.h"

#include "../../Engine/AABBComponent.h"
#include "../../Engine/DebugTextComponent.h"
#include "../../Engine/GridTransformComponent.h"
#include "../../Engine/GridWorldComponent.h"
#include "../../Engine/KBInputComponent.h"
#include "../../Engine/NPCBlackboardComponent.h"
#include "../../Engine/PadInputComponent.h"
#include "../../Engine/PlaneComponent.h"
#include "../../Engine/PlayerComponent.h"
#include "../../Engine/RigidBodyComponent.h"
#include "../../Engine/SpriteComponent.h"
#include "../../Engine/TransformComponent.h"

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

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<PlayerControlSystem>()->Init(renderInit);
	world->RegisterSystem<NPCControlSystem>()->Init();
	world->RegisterSystem<NPCSensorSystem>()->Init();
	world->RegisterSystem<PhysicsSystem>()->Init(renderInit);
	world->RegisterSystem<BoxCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<PlaneCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<SpriteRenderSystem>()->Init(renderInit);

	// Create GLOBAL components

	world->SetGlobalComponent<GridWorldComponent>( { Rect2D{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32.0f } );

	// Create test world entities
	auto createSprite = [](World& w, ResourceID resID, Vector3f pos)
	{
		EntityID e = w.CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createGridSprite = [](World& w, ResourceID resID, Vector2i pos)
	{
		EntityID e = w.CreateEntity();
		const GridTransformComponent t(pos);
		w.AddComponent<GridTransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createSpriteWithPhysics = [](World& w, ResourceID resID, Vector3f pos, Vector2f boxSize, Vector2f boxOffset)
	{
		EntityID e = w.CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, t);
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

	auto ufoEntity = createSpriteWithPhysics(*world, resID_ufo, { 200, 200, 200 }, { 158, 48 }, { 79, 24 });
	world->AddComponent<NPCBlackboardComponent>(ufoEntity);

	return world;
}
