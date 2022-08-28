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
#include "Systems/BlockDropperSystem.h"

#include "Components/SpeciesComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BlockDropperComponent.h"

EntityBuilder<SpeciesIdentity<Species::PLANT>> BlockoLifeWorldBuilder::m_PlantBuilder;
EntityBuilder<SpeciesIdentity<Species::HERBIVORE>> BlockoLifeWorldBuilder::m_HerbivoreBuilder;
EntityBuilder<SpeciesIdentity<Species::CARNIVORE>> BlockoLifeWorldBuilder::m_CarnivoreBuilder;

std::shared_ptr<World> BlockoLifeWorldBuilder::BuildWorld(std::shared_ptr<SDLRenderManager> renderMan)
{	
	// LOAD RESOURCES
	// TODO resource loader system should load separately and more automated,
	// but we'd need to take the res IDs into the entity builders via metadata?
	ResourceID creatureSprites[SpeciesCount]{};
	creatureSprites[to_underlying(Species::PLANT)] = m_PlantBuilder.LoadResources(*renderMan);
	creatureSprites[to_underlying(Species::HERBIVORE)] = m_HerbivoreBuilder.LoadResources(*renderMan);
	creatureSprites[to_underlying(Species::CARNIVORE)] = m_CarnivoreBuilder.LoadResources(*renderMan);
	static_assert(SpeciesCount == 3, "Needs updating");

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
	world->RegisterComponent<BlockDropperComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<GridSpriteRenderSystem>()->Init(renderInit);
	world->RegisterSystem<GameOfLifeSystem>()->Init(renderInit);
	world->RegisterSystem<CreatureSystem>()->Init(renderInit);
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<Camera2DSystem>()->Init(renderInit);
	world->RegisterSystem<BlockDropperSystem>()->Init(renderInit);

	// Create GLOBAL components
	// TODO FIXME HACK worakround for bug when removing entity 0 since the global components are stored in the same place!
	EntityID globalHack = world->CreateEntity();
	assert(globalHack == 0);
	world->SetGlobalComponent<Camera2DComponent>();
	world->SetGlobalComponent<GridWorldComponent>( { Rect2f{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32 } );


	// Create player (just for camera for now)
	// TODO use EntityBuilder
	const Species dropperSpecies = Species::PLANT;

	EntityID playerEntity = world->CreateEntity();
	world->AddComponent<KBInputComponent>(playerEntity);
	world->AddComponent<PadInputComponent>(playerEntity);
	world->AddComponent<GridTransformComponent>(playerEntity, {{0, 0}});
	world->AddComponent<BlockDropperComponent>(playerEntity, {dropperSpecies});
	// TODO test hack - needs to be switchable
	const SpriteComponent dropperSprite (
		creatureSprites[to_underlying(dropperSpecies)],
		100);
	world->AddComponent<SpriteComponent>(playerEntity, dropperSprite);

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
					m_PlantBuilder.Build(*world, { x, y });
				}
			}
		}
	}
	else
	{
		m_PlantBuilder.Build(*world, { 5, 4 });
		m_PlantBuilder.Build(*world, { 5, 5 });
		m_PlantBuilder.Build(*world, { 6, 5 });
		m_PlantBuilder.Build(*world, { 6, 6 });
		m_PlantBuilder.Build(*world, { 5, 6 });
		m_PlantBuilder.Build(*world, { 6, 7 });
	}

	m_HerbivoreBuilder.Build(*world, { 5, 21 });
	m_HerbivoreBuilder.Build(*world, { 8, 21 });

	m_CarnivoreBuilder.Build(*world, { 20, 22 });

	return world;
}
