#include "BlockoLifeWorldBuilder.h"

// FIXME proper include paths from game builders

#include "../../Engine/World.h"

#include "../../Engine/BoxCollisionSystem.h"
#include "../../Engine/GridSpriteRenderSystem.h"
#include "../../Engine/GridTransformComponent.h"
#include "../../Engine/NPCControlSystem.h"
#include "../../Engine/NPCSensorSystem.h"
#include "../../Engine/PhysicsSystem.h"
#include "../../Engine/PlaneCollisionSystem.h"
#include "../../Engine/PlayerControlSystem.h"
#include "../../Engine/SDLInputSystem.h"
#include "../../Engine/SpriteRenderSystem.h"

#include "../../Engine/AABBComponent.h"
#include "../../Engine/DebugTextComponent.h"
#include "../../Engine/GridWorldComponent.h"
#include "../../Engine/KBInputComponent.h"
#include "../../Engine/NPCBlackboardComponent.h"
#include "../../Engine/PadInputComponent.h"
#include "../../Engine/PlaneComponent.h"
#include "../../Engine/PlayerComponent.h"
#include "../../Engine/RigidBodyComponent.h"
#include "../../Engine/SpriteComponent.h"
#include "../../Engine/TransformComponent.h"

std::shared_ptr<World> BlockoLifeWorldBuilder::BuildWorld(std::shared_ptr<SDLRenderManager> renderMan)
{	
	// LOAD RESOURCES
	// TODO resource loading should be separate from world building
	ResourceID resID_plant = renderMan->LoadTexture("assets/sprites/plant_32.png");
	assert(resID_plant != ResourceID_Invalid);
	ResourceID resID_herbivore = renderMan->LoadTexture("assets/sprites/herbivore_32.png");
	assert(resID_herbivore != ResourceID_Invalid);
	ResourceID resID_carnivore = renderMan->LoadTexture("assets/sprites/carnivore_32.png");
	assert(resID_carnivore != ResourceID_Invalid);

	// BUILD WORLD
	std::shared_ptr<World> world = std::make_shared<World>();

	// TODO Currently we get a crash if we add a component we forgot to register. Can we check at compile time?
	world->RegisterComponent<TransformComponent>();
	world->RegisterComponent<GridTransformComponent>();
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
	world->RegisterSystem<GridSpriteRenderSystem>()->Init(renderInit);

	// Create GLOBAL components
	world->SetGlobalComponent<GridWorldComponent>( { Rect2D{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32.0f } );

	// Load resources and create test world entities
	auto createGridSprite = [](World& w, ResourceID resID, Vector2i pos)
	{
		EntityID e = w.CreateEntity();
		const GridTransformComponent t(pos);
		w.AddComponent<GridTransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};


	// Crwea
	createGridSprite(*world, resID_plant, { 5, 5 });
	createGridSprite(*world, resID_plant, { 6, 5 });
	createGridSprite(*world, resID_plant, { 6, 6 });
	createGridSprite(*world, resID_plant, { 5, 6 });
	createGridSprite(*world, resID_herbivore, { 8, 8 });
	createGridSprite(*world, resID_herbivore, { 8, 9 });
	createGridSprite(*world, resID_carnivore, { 9, 2 });

	return world;
}
