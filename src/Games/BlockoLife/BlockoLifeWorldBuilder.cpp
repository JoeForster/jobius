#include "BlockoLifeWorldBuilder.h"

// Lib includes
#include <array>
#include <numeric>

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

#pragma optimize("", off)

EntityBuilder<SpeciesIdentity<Species::PLANT>> BlockoLifeWorldBuilder::m_PlantBuilder;
EntityBuilder<SpeciesIdentity<Species::HERBIVORE>> BlockoLifeWorldBuilder::m_HerbivoreBuilder;
EntityBuilder<SpeciesIdentity<Species::CARNIVORE>> BlockoLifeWorldBuilder::m_CarnivoreBuilder;
EntityBuilder<SpeciesIdentity<Species::NO_SPECIES>> BlockoLifeWorldBuilder::m_EmptyBuilder;

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
	world->AddComponent<GridTransformComponent>(playerEntity, {{14, 10}});
	world->AddComponent<BlockDropperComponent>(playerEntity, {dropperSpecies});
	// TODO test hack - needs to be switchable
	SpriteComponent dropperSprite (
		creatureSprites[to_underlying(dropperSpecies)],
		100);
	world->AddComponent<SpriteComponent>(playerEntity, std::move(dropperSprite));

	// Create creature sprites
	enum class Scenario
	{
		FIXED,
		RANDOM_SMALL,
		RANDOM_MASSIVE
	};
	// TODO scenario from config?
	static constexpr Scenario scenario = Scenario::RANDOM_MASSIVE;

	std::map<int, std::map<int, Species>> creatureSpawns;
	Rect2i limits ( {0, 0}, {0, 0} );
	auto AddSpawn = [&](Species species, const Vector2i& pos)
	{
		assert(species < Species::SPECIES_COUNT);

		if (creatureSpawns.find(pos.x) == creatureSpawns.cend())
			creatureSpawns[pos.x] = {{ pos.y, species }};
		else
			creatureSpawns[pos.x].insert({pos.y, species});

		//creatureSpawns[pos.x] = species;
		if (pos.x < limits.min.x) limits.min.x = pos.x;
		if (pos.y < limits.min.y) limits.min.y = pos.y;
		if (pos.x > limits.max.x) limits.max.x = pos.x;
		if (pos.y > limits.max.y) limits.max.y = pos.y;
	};


	switch (scenario)
	{
		case Scenario::FIXED:
		{
			AddSpawn(Species::PLANT, { 5, 4 });
			AddSpawn(Species::PLANT, { 5, 5 });
			AddSpawn(Species::PLANT, { 6, 5 });
			AddSpawn(Species::PLANT, { 6, 6 });
			AddSpawn(Species::PLANT, { 5, 6 });
			AddSpawn(Species::PLANT, { 6, 7 });
			
			AddSpawn(Species::HERBIVORE, { 5, 11 });
			AddSpawn(Species::HERBIVORE, { 8, 11 });
		
			m_CarnivoreBuilder.Build(*world, { 12, 12 });
			
			break;
		}
		case Scenario::RANDOM_SMALL:
		{
			static constexpr int RANDOM_PLANTS_PROBABILITY_PERCENT = 75;

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

			AddSpawn(Species::HERBIVORE, { 5, 21 });
			AddSpawn(Species::HERBIVORE, { 8, 21 });
			AddSpawn(Species::HERBIVORE, { 9, 22 });
			AddSpawn(Species::HERBIVORE, { 2, 3 });
			AddSpawn(Species::HERBIVORE, { 7, 4 });
			AddSpawn(Species::HERBIVORE, { 18, 3 });

			m_CarnivoreBuilder.Build(*world, { 24, 24 });
			m_CarnivoreBuilder.Build(*world, { 1, 1 });

			break;
		}
		case Scenario::RANDOM_MASSIVE:
		{
			// Probability distribution for each type
			static constexpr auto SpawnProbabilities = std::to_array<int>({
				40, // PLANT
				10, // HERBIVORE
				1 // CARNIVORE
			});
			static_assert(SpawnProbabilities.size() == SpeciesCount, "SpawnProbabilities needs updating!");
			static constexpr auto totalProbs = std::accumulate(SpawnProbabilities.begin(), SpawnProbabilities.end(), 0, std::plus<int>());
			static_assert(totalProbs >= 0 && totalProbs <= 100);
			
			Rect2i spawnArea { { 0, 0 }, { 50, 40 } };
			for (int y = spawnArea.min.y; y < spawnArea.max.y; ++y)
			{
				for (int x = spawnArea.min.x; x < spawnArea.max.x; ++x)
				{ 
					int spawnRoll = rand() % 100;

					int probSum = 0;
					for (int speciesIndex = 0; speciesIndex < SpeciesCount; ++speciesIndex)
					{
						probSum += SpawnProbabilities[speciesIndex];
						if (spawnRoll <= probSum)
						{
							// TODO no need for switch if mapped by type
							switch ((Species)speciesIndex)
							{
								case Species::PLANT: AddSpawn(Species::PLANT, { x, y }); break;
								case Species::HERBIVORE: AddSpawn(Species::HERBIVORE, { x, y }); break;
								case Species::CARNIVORE: AddSpawn(Species::CARNIVORE, { x, y }); break;
							}
							break;
						}
					}
				}
			}
			break;
		}
		default:
			assert(false); // TODO compile-time would be nice!

	}

	// TODO this should be the same logic as growing/shrinking after each tick?
	// Actually create the grid squares, blank or with creature
	// Need to fill out adjacent blocks too to make room for growth
	limits.min.x -= 1;
	limits.min.y -= 1;
	limits.max.x += 1;
	limits.max.y += 1;

	for (int y = limits.min.y+1; y < limits.max.y-1; ++y)
	{
		for (int x = limits.min.x+1; x < limits.max.x-1; ++x)
		{
			const Vector2i spawnPos {x, y};
			const auto& itSpawnCol = creatureSpawns.find(x);
			if (itSpawnCol != creatureSpawns.cend())
			{
				const auto& itSpawn = itSpawnCol->second.find(y);
				if (itSpawn != itSpawnCol->second.cend())
				{
					BuildEntity(*world, itSpawn->second, spawnPos);
				}
				else
				{
					// TODO this does not quite work
					m_EmptyBuilder.Build(*world, spawnPos);
				}
			}
			else
			{
				m_EmptyBuilder.Build(*world, spawnPos);
			}
		}
	}

	// All entities are constructed, so we can now refresh the signatures
	world->UpdateEntitySets();

	return world;
}
