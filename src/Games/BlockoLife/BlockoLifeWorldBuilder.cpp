#include "BlockoLifeWorldBuilder.h"

// Engine includes
#include "World.h"

#include "BoxCollisionSystem.h"
#include "GridSpriteRenderSystem.h"
#include "GridTransformComponent.h"
#include "NPCControlSystem.h"
#include "NPCSensorSystem.h"
#include "PhysicsSystem.h"
#include "PlaneCollisionSystem.h"
#include "PlayerControlSystem.h"
#include "SDLInputSystem.h"
#include "SpriteRenderSystem.h"

#include "AABBComponent.h"
#include "DebugTextComponent.h"
#include "GridWorldComponent.h"
#include "KBInputComponent.h"
#include "NPCBlackboardComponent.h"
#include "PadInputComponent.h"
#include "PlaneComponent.h"
#include "PlayerComponent.h"
#include "RigidBodyComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

// BlockoLife includes
#include "Systems/GameOfLifeSystem.h"

#include "Components/SpeciesComponent.h"

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
	world->RegisterComponent<SpeciesComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	// TODO remove unused
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<PlayerControlSystem>()->Init(renderInit);
	world->RegisterSystem<NPCControlSystem>()->Init();
	world->RegisterSystem<NPCSensorSystem>()->Init();
	world->RegisterSystem<PhysicsSystem>()->Init(renderInit);
	world->RegisterSystem<BoxCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<PlaneCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<SpriteRenderSystem>()->Init(renderInit);
	world->RegisterSystem<GridSpriteRenderSystem>()->Init(renderInit);
	world->RegisterSystem<GameOfLifeSystem>()->Init(renderInit);

	// Create GLOBAL components
	// TODO FIXME HACK worakround for bug when removing entity 0 since the global components are stored in the same place!
	EntityID globalHack = world->CreateEntity();
	assert(globalHack == 0);
	world->SetGlobalComponent<GridWorldComponent>( { Rect2D{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32.0f } );

	// Load resources and create test world entities
	auto createGridSprite = [](World& w, ResourceID resID, Vector2i pos, Species species)
	{
		EntityID e = w.CreateEntity();
		const GridTransformComponent t(pos);
		w.AddComponent<GridTransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		w.AddComponent<SpeciesComponent>(e, species);
		return e;
	};


	// Create test sprites
	createGridSprite(*world, resID_plant, { 5, 4 }, Species::PLANT);
	createGridSprite(*world, resID_plant, { 5, 5 }, Species::PLANT);
	createGridSprite(*world, resID_plant, { 6, 5 }, Species::PLANT);
	createGridSprite(*world, resID_plant, { 6, 6 }, Species::PLANT);
	createGridSprite(*world, resID_plant, { 5, 6 }, Species::PLANT);
	createGridSprite(*world, resID_plant, { 6, 7 }, Species::PLANT);

	createGridSprite(*world, resID_herbivore, { 8, 8 }, Species::HERBIVORE);
	createGridSprite(*world, resID_herbivore, { 8, 9 }, Species::HERBIVORE);

	createGridSprite(*world, resID_carnivore, { 9, 2 }, Species::CARNIVORE);

	return world;
}
