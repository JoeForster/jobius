#include "BlockoLifeWorldBuilder.h"

// Engine includes
#include "World.h"
#include "SDLRenderManager.h"

#include "GridSpriteRenderSystem.h"
#include "GridTransformComponent.h"
#include "SDLInputSystem.h"
#include "Camera2DSystem.h"

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
#include "Camera2DComponent.h"

// BlockoLife includes
#include "Systems/GameOfLifeSystem.h"
#include "Systems/CreatureSystem.h"

#include "Components/SpeciesComponent.h"
#include "Components/HealthComponent.h"

EntityBuilder<SpeciesIdentity<Species::PLANT>> BlockoLifeWorldBuilder::plantBuilder;
EntityBuilder<SpeciesIdentity<Species::HERBIVORE>> BlockoLifeWorldBuilder::herbivoreBuilder;
EntityBuilder<SpeciesIdentity<Species::CARNIVORE>> BlockoLifeWorldBuilder::carnivoreBuilder;

std::shared_ptr<World> BlockoLifeWorldBuilder::BuildWorld(std::shared_ptr<SDLRenderManager> renderMan)
{	
	// LOAD RESOURCES
	// TODO resource loader system should load separately and more automated,
	// but we'd need to take the res IDs into the entity builders via metadata?
	plantBuilder.LoadResources(*renderMan);
	herbivoreBuilder.LoadResources(*renderMan);
	carnivoreBuilder.LoadResources(*renderMan);

	// BUILD WORLD
	std::shared_ptr<World> world = std::make_shared<World>();

	// TODO Currently we get a crash if we add a component we forgot to register. Can we check at compile time?
	// TODO don't register components we don't need
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
	world->RegisterComponent<HealthComponent>();
	world->RegisterComponent<Camera2DComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<GridSpriteRenderSystem>()->Init(renderInit);
	world->RegisterSystem<GameOfLifeSystem>()->Init(renderInit);
	world->RegisterSystem<CreatureSystem>()->Init(renderInit);
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<Camera2DSystem>()->Init(renderInit);

	// Create GLOBAL components
	// TODO FIXME HACK worakround for bug when removing entity 0 since the global components are stored in the same place!
	EntityID globalHack = world->CreateEntity();
	assert(globalHack == 0);
	world->SetGlobalComponent<Camera2DComponent>();
	world->SetGlobalComponent<GridWorldComponent>( { Rect2f{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32 } );


	// Create player (just for camera for now)
	// TODO use EntityBuilder
	EntityID playerEntity = world->CreateEntity();
	world->AddComponent<KBInputComponent>(playerEntity);
	world->AddComponent<PadInputComponent>(playerEntity);

	// Create creature sprites
	static constexpr bool RANDOM_PLANTS = true;
	static constexpr int RANDOM_PLANTS_PROBABILITY_PERCENT = 75;

	if (RANDOM_PLANTS)
	{
		Rect2i plantArea { { 5, 5 }, { 25, 20 } };
		for (int y = plantArea.min.y; y < plantArea.max.y; ++y)
		{
			for (int x = plantArea.min.x; x < plantArea.max.x; ++x)
			{
				int spawnRoll = rand() % 100;
				if (spawnRoll < RANDOM_PLANTS_PROBABILITY_PERCENT)
				{
					//createGridSprite(*world, resID_plant, { x, y }, Species::PLANT);
					plantBuilder.Build(*world, { x, y });
				}
			}
		}
	}
	else
	{
		plantBuilder.Build(*world, { 5, 4 });
		plantBuilder.Build(*world, { 5, 5 });
		plantBuilder.Build(*world, { 6, 5 });
		plantBuilder.Build(*world, { 6, 6 });
		plantBuilder.Build(*world, { 5, 6 });
		plantBuilder.Build(*world, { 6, 7 });
	}

	herbivoreBuilder.Build(*world, { 5, 21 });
	herbivoreBuilder.Build(*world, { 8, 21 });

	carnivoreBuilder.Build(*world, { 20, 22 });

	return world;
}
