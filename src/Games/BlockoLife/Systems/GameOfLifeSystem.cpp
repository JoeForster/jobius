#include "GameOfLifeSystem.h"

#include <vector>
#include <string>
#include <format>
#include <algorithm>
#include <chrono>
#include <iostream>

#include "Vector.h"

#include "GridTransformComponent.h"
#include "Components/SpeciesComponent.h"
#include "Components/HealthComponent.h"

#include "World.h"
#include "SDLRenderManager.h"

#include "BlockoLifeWorldBuilder.h"

constexpr bool DESTROY_PLANT_ON_EATEN = false;

#define DEBUG_PRINT_ON 0

#if DEBUG_PRINT_ON
#	define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#	define DEBUG_PRINT(...)
#endif

void GameOfLifeSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_SPRITE);
	sysSignature.set((size_t)ComponentType::CT_BL_SPECIES);
	sysSignature.set((size_t)ComponentType::CT_BL_HEALTH);
	m_ParentWorld->SetSystemSignatures<GameOfLifeSystem>(sysSignature);
}


void GameOfLifeSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// HACK
	constexpr float tickPeriod = 1.0f;
	static float timer = 0.0f;
	timer += deltaSecs;
	if (timer >= tickPeriod)
	{
		timer = 0.0f;
		Tick();
	}
	
}



struct CreatureCache
{
	EntityID entityID = INVALID_ENTITY_ID;
	Species species = Species::SPECIES_COUNT;
	int health = 0;
	uint8_t neighbourHealthTotals[SpeciesCount] = { 0 };
	EntityID parentEntityIDs[(size_t)Species::SPECIES_COUNT] = { INVALID_ENTITY_ID }; // hacky, it's just the first entity found of each species if any

	bool IsAlive() const { return entityID != INVALID_ENTITY_ID && health > 0; }
};

enum class GridDir
{
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,

	COUNT
};
constexpr size_t GridDirCount = to_underlying(GridDir::COUNT);
static_assert(to_underlying(GridDir::N) == 0);
static_assert(GridDirCount == 8);

static constexpr Vector2i GridDirToVec[GridDirCount] = {
	{ 0, -1 },		// GridDir::N
	{ 1, -1 },		// GridDir::NE
	{ 1,  0 },		// GridDir::E
	{ 1,  1 },		// GridDir::SE
	{ 0,  1 },		// GridDir::S
	{-1,  1 },		// GridDir::SW
	{-1,  0 },		// GridDir::W
	{-1, -1 },		// GridDir::NW
};

static GridDir VecToGridDir(const Vector2i& dirVec)
{
	if (dirVec.x < 0)
	{
		if (dirVec.y < 0) return GridDir::NW;
		else if (dirVec.y > 0) return GridDir::SW;
		else return GridDir::W;
	}
	else if (dirVec.x > 0)
	{
		if (dirVec.y < 0) return GridDir::NE;
		else if (dirVec.y > 0) return GridDir::SE;
		else return GridDir::E;
	}
	else// if (dirVec.x == 0)
	{
		assert(dirVec.x == 0);
		if (dirVec.y < 0) return GridDir::N;
		else if (dirVec.y > 0) return GridDir::S;
	}

	assert(dirVec == Vector2i::ZERO);
	return GridDir::COUNT; // COUNT indicates same position
}

uint8_t GameOfLifeSystem::CountNeighbours(Array2D<CreatureCache>& cachedEntities, int x, int y, Species s)
{
	uint8_t n = 0;
	for (const Vector2i& dirVec : GridDirToVec)
	{
		const Vector2i checkPos = Vector2i(x, y) + dirVec;
		CreatureCache& cacheAt = cachedEntities.At(checkPos);
		if (cacheAt.species == s)
		{
			++n;
		}
	}
	return n;
};

// TODO temp splitting into these functions - needs better structure
// Lots of duplication and ideally each should be a separate system that just sets a "move intent"
// and central place does the actual moving to resolve blockage etc?

void GameOfLifeSystem::Tick_Move_Herbivore(Array2D<CreatureCache>& cachedEntities, CreatureCache& cache, int x, int y, std::set<EntityID>& entitiesToRemove)
{
	const EntityID movingEntity = cache.entityID;
	assert(cache.species == Species::HERBIVORE);

	DEBUG_PRINT("Move HERBIVORE %d\n", movingEntity);
	auto& health = m_ParentWorld->GetComponent<HealthComponent>(movingEntity);

	GridDir possibleDirs[GridDirCount];
	int numPossibleDirs = 0;

	bool didEat = false;

	int mostPlants = 0;
	int bestDirIndex = 0;
	for (int dir = 0; dir < GridDirCount; ++dir)
	{
		const Vector2i checkPos = Vector2i(x, y) + GridDirToVec[dir];

		CreatureCache& adjacentCreature = cachedEntities.At(checkPos.x, checkPos.y);
		if (adjacentCreature.entityID != INVALID_ENTITY_ID)
		{
			// BLOCKED
			if (adjacentCreature.species == Species::PLANT)
			{
				// EAT if plant
				constexpr int healthGain = 10;
				const int healthBefore = health.m_Health;
				health.ModHealth(healthGain);
				const int healthAfter = health.m_Health;

				DEBUG_PRINT("Entity[HERBIVORE] %d at (%d, %d) eat plant %d at (%d, %d)! Health %d -> %d\n",
					movingEntity, x, y, adjacentCreature.entityID, checkPos.x, checkPos.y, healthBefore, healthAfter);

				if (DESTROY_PLANT_ON_EATEN)
				{
					entitiesToRemove.insert(adjacentCreature.entityID);
					adjacentCreature = CreatureCache();
				}						

				didEat = true;
			}
		}
		else
		{
			possibleDirs[numPossibleDirs++] = (GridDir)dir;
			int numPlants = CountNeighbours(cachedEntities, x, y-1, Species::PLANT);
			if (numPlants > mostPlants)
			{
				mostPlants = numPlants;
				bestDirIndex = dir;
			}
		}
	}

	if (numPossibleDirs == 0)
	{
		DEBUG_PRINT("Entity %d at (%d, %d) is BLOCKED!\n", movingEntity, x, y);
	}

	if (!didEat)
	{
		constexpr int healthLoss = -1;
		const int healthBefore = health.m_Health;
		health.ModHealth(healthLoss);
		const int healthAfter = health.m_Health;
		DEBUG_PRINT("Entity[HERBIVORE] %d at (%d, %d) didn't eat - Health %d -> %d\n",
			movingEntity, x, y, healthBefore, healthAfter);
		if (healthAfter == 0)
		{
			DEBUG_PRINT("Entity[HERBIVORE] %d at (%d, %d) didn't eat - ZERO HEALTH, STARVED!",
				movingEntity, x, y);
			entitiesToRemove.insert(movingEntity);
			cache = CreatureCache();
			numPossibleDirs = 0;
		}
	}

	if (numPossibleDirs != 0)
	{
		if (mostPlants == 0)
		{
			const int randomDirIndex = rand() % numPossibleDirs;
			bestDirIndex = to_underlying(possibleDirs[randomDirIndex]);
			DEBUG_PRINT("Entity[HERBIVORE] %d at (%d, %d) Pick random dir: %d\n", movingEntity, x, y, bestDirIndex);
 		}
		else
		{
			DEBUG_PRINT("Entity[HERBIVORE] %d at (%d, %d) Pick best dir %d\n", movingEntity, x, y, bestDirIndex);
		}		

		auto& thisTransform = m_ParentWorld->GetComponent<GridTransformComponent>(movingEntity);
		if (thisTransform.m_Pos.x != x || thisTransform.m_Pos.y != y)
		{
			printf("OOPS this object %d is at unexpected position - original (%d, %d) in cache (%d, %d)\n",
				movingEntity, thisTransform.m_Pos.x, thisTransform.m_Pos.y, x, y);
			assert(false);
		}
		const Vector2i moveIntoPos = thisTransform.m_Pos + GridDirToVec[bestDirIndex];

		auto& target = cachedEntities.At(moveIntoPos.x, moveIntoPos.y);

		// UGH we have two places where this info lives due to the cache - this can be better
		// once we use the actual component storage rather than separate cache.
		//assert(!target.beingMovedInto);
		target = cache;
		cache = CreatureCache();
		thisTransform.m_Pos = moveIntoPos;
	}
}

void GameOfLifeSystem::Tick_Move_Carnivore(Array2D<CreatureCache>& cachedEntities, CreatureCache& cache, int x, int y, std::set<EntityID>& entitiesToRemove)
{
	const EntityID movingEntity = cache.entityID;
	assert(cache.species == Species::CARNIVORE);

	DEBUG_PRINT("Move CARNIVORE %d\n", movingEntity);
	auto& health = m_ParentWorld->GetComponent<HealthComponent>(movingEntity);

	bool didEat = false;

	// TODO more interesting logic for carnivore here - e.g. following trails,
	// resting when health over threshold, hunting grounds... Good candidate to test behaviour tree?

	// Scan for closest herbivore to eat if health is low enough
	int closestTargetDist = -1;
	Vector2i targetPos;
	if (health.m_Health < health.m_MaxHealth/2)
	{
		for (EntityID e : GetEntities())
		{
			auto& t = m_ParentWorld->GetComponent<GridTransformComponent>(e);
			auto& s = m_ParentWorld->GetComponent<SpeciesComponent>(e);
			if (s.m_Species == Species::HERBIVORE)
			{
				const int thisDist = abs(t.m_Pos.x - x) + abs(t.m_Pos.y - y);
				if (closestTargetDist == -1 || thisDist < closestTargetDist)
				{
					closestTargetDist = thisDist;
					targetPos = t.m_Pos;
				}
			}
		}
	}




	// Eat target if close enough
	GridDir moveDir = GridDir::COUNT;
	if (closestTargetDist >= 0)
	{
		Vector2i posToTarget = targetPos;
		posToTarget.x -= x;
		posToTarget.y -= y;

		if (abs(posToTarget.x) <= 1 && abs(posToTarget.y) <= 1)
		{
			CreatureCache& adjacentCreature = cachedEntities.At(targetPos.x, targetPos.y);
			if (adjacentCreature.species == Species::HERBIVORE)
			{
				// TODO bug here not expected species sometimes?
				//assert (adjacentCreature.species == Species::HERBIVORE);
				assert (adjacentCreature.entityID != INVALID_ENTITY_ID);

				// EAT if herbivore
				constexpr int healthGain = 20;
				const int healthBefore = health.m_Health;
				health.ModHealth(healthGain);
				const int healthAfter = health.m_Health;

				DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) eat HERBIVORE %d at (%d, %d)! Health %d -> %d\n",
					movingEntity, x, y, adjacentCreature.entityID, targetPos.x, targetPos.y, healthBefore, healthAfter);

				entitiesToRemove.insert(adjacentCreature.entityID);
				adjacentCreature = CreatureCache();

				didEat = true;
			}
			
		}
		// Otherwise try to move towards the target
		else
		{
			moveDir = VecToGridDir(posToTarget);
		}
	}

	if (!didEat)
	{
		constexpr int healthLoss = -1;
		const int healthBefore = health.m_Health;
		health.ModHealth(healthLoss);
		const int healthAfter = health.m_Health;
		DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) didn't eat - Health %d -> %d\n",
			movingEntity, x, y, healthBefore, healthAfter);
		if (healthAfter == 0)
		{
			DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) didn't eat - ZERO HEALTH, STARVED!",
				movingEntity, x, y);
			entitiesToRemove.insert(movingEntity);
			cache = CreatureCache();
			return; // TODO smelly late early return
		}
	}

	if (moveDir == GridDir::COUNT)
	{
		DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) not moving\n", movingEntity, x, y);
	}
	else
	{
		DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) Move dir %d\n", movingEntity, x, y, moveDir);

		auto& thisTransform = m_ParentWorld->GetComponent<GridTransformComponent>(movingEntity);
		if (thisTransform.m_Pos.x != x || thisTransform.m_Pos.y != y)
		{
			DEBUG_PRINT("OOPS this object %d is at unexpected position - original (%d, %d) in cache (%d, %d)\n",
				movingEntity, thisTransform.m_Pos.x, thisTransform.m_Pos.y, x, y);
			assert(false);
		}
		const Vector2i moveIntoPos = thisTransform.m_Pos + GridDirToVec[(int)moveDir];

		auto& target = cachedEntities.At(moveIntoPos.x, moveIntoPos.y);
		if (target.entityID == INVALID_ENTITY_ID)
		{
			// UGH we have two places where this info lives due to the cache - this can be better
			// once we use the actual component storage rather than separate cache.
			//assert(!target.beingMovedInto);
			target = cache;
			cache = CreatureCache();
			thisTransform.m_Pos = moveIntoPos;
		}
		else
		{
			DEBUG_PRINT("Entity[CARNIVORE] %d at (%d, %d) BLOCKED moving into (%d, %d)\n", movingEntity, x, y, moveIntoPos.x, moveIntoPos.y);
		}
	}
}

// NAIVE implementation of Conway's Game of Life in ECS (but not really in ECS)
// TODO needs heavy tidy-up
void GameOfLifeSystem::Tick()
{
	using std::chrono::steady_clock;

	steady_clock::time_point start = steady_clock::now(); 

	// Solution 1: an for each living creature.
	// TODO: Inefficient lookup for now, may need either a cache or a better query system
	// (we'd like to be able to index into components, or adjacent ones, I guess?)
	Rect2i limits ( {0, 0}, {0, 0} );

	if (GetEntities().empty())
	{
		printf("\nTICK no entities!\n");
	}

	for (EntityID e : GetEntities())
	{
		auto t = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		if (t.m_Pos.x < limits.min.x) limits.min.x = t.m_Pos.x;
		if (t.m_Pos.y < limits.min.y) limits.min.y = t.m_Pos.y;
		if (t.m_Pos.x > limits.max.x) limits.max.x = t.m_Pos.x;
		if (t.m_Pos.y > limits.max.y) limits.max.y = t.m_Pos.y;
	
		// Need to consider adjacent blocks too
		limits.min.x -= 1;
		limits.min.y -= 1;
		limits.max.x += 1;
		limits.max.y += 1;
	}

	printf("\nTICK entities(%zu) limits min(%d %d) max(%d %d)\n", GetEntities().size(), limits.min.x, limits.min.y, limits.max.x, limits.max.y);

	assert(limits.max.y > limits.min.y);
	assert(limits.max.x > limits.min.x);
	assert(limits.min.x < 0);
	assert(limits.min.y < 0);

	size_t num_rows = (size_t)(limits.max.y - limits.min.y) + 1;
	size_t num_columns = (size_t)(limits.max.x - limits.min.x) + 1;
	size_t num_cells = num_rows * num_columns;	
	size_t x_offset = (size_t)(0 - limits.min.x);
	size_t y_offset = (size_t)(0 - limits.min.y);

	// TODO we're kind of bypassing the ECS here with custom storage - we need a way of storing components
	// in the right order OR so we can index into adjacent cells!
	// NOTE yes we could avoid remaking this each tick, but would need to put in support for vector-like grow/shrink
	// due to the unbounded world. And then we might as well make this use ECS properly.
	auto cachedEntities = Array2D<CreatureCache>(num_columns, num_rows, x_offset, y_offset);

	for (EntityID e : GetEntities())
	{
		auto& transform = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		auto& species = m_ParentWorld->GetComponent<SpeciesComponent>(e);
		auto& health = m_ParentWorld->GetComponent<HealthComponent>(e);

		size_t index = transform.m_Pos.y*num_rows + transform.m_Pos.x;
		CreatureCache& cache = cachedEntities.At(transform.m_Pos.x, transform.m_Pos.y);
		assert(cache.entityID == INVALID_ENTITY_ID);
		cache.entityID = e;
		cache.species = species.m_Species;
		cache.health = health.m_Health;;
	}

	std::set<EntityID> entitiesToRemove;

	// Move pass
	for (EntityID movingEntity : GetEntities())
	{
		if (entitiesToRemove.contains(movingEntity))
		{
			continue;
		}

		auto& originTransform = m_ParentWorld->GetComponent<GridTransformComponent>(movingEntity);
		const int x = originTransform.m_Pos.x;
		const int y = originTransform.m_Pos.y;
		CreatureCache& cache = cachedEntities.At(x, y);
		assert(cache.entityID == movingEntity);
		assert(cache.IsAlive()); // No living entities should have been left between ticks!

		// HACK species-specific behaviour should be in its own system
		switch (cache.species)
		{
			case Species::HERBIVORE:
				Tick_Move_Herbivore(cachedEntities, cache, x, y, entitiesToRemove);
				break;
			case Species::CARNIVORE:
				Tick_Move_Carnivore(cachedEntities, cache, x, y, entitiesToRemove);
				break;
			default:
				break;
		}
	}


	// Reproduction scan pass
	// First count adjacency to see what will grow/die
	// NOTE we leave out the "border areas" here as they have been added above and are blank.. This could be clearer.
	for (int y = limits.min.y+1; y < limits.max.y-1; ++y)
	{
		for (int x = limits.min.x+1; x < limits.max.x-1; ++x)
		{
			CreatureCache& creature = cachedEntities.At(x, y);
			EntityID thisEntity = creature.entityID;
			uint8_t numPlantNeighbours = CountNeighbours(cachedEntities, x, y, Species::PLANT);
			DEBUG_PRINT("neighbours at (%d, %d): %d\n", x, y, numNeighbours);
			// Scan neighbour species for reproduction 
			for (const Vector2i& dirVec : GridDirToVec)
			{
				const Vector2i checkPos = Vector2i(x, y) + dirVec;
				CreatureCache& cacheAt = cachedEntities.At(checkPos);
				if (cacheAt.entityID != INVALID_ENTITY_ID)
				{
					creature.neighbourHealthTotals[(int)cacheAt.species] += cacheAt.health;
					if (creature.parentEntityIDs[(int)cacheAt.species] == INVALID_ENTITY_ID)
					{
						creature.parentEntityIDs[(int)cacheAt.species] = cacheAt.entityID;
					}
				}
			}
		}
	}

	// Then do the grow/die pass
	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			CreatureCache& cache = cachedEntities.At(x, y);
			if (!cache.IsAlive())
			{
				assert(cache.species == Species::SPECIES_COUNT);
				if (cache.neighbourHealthTotals[(int)Species::PLANT] == 3)
				{
					EntityID parent = cache.parentEntityIDs[(int)Species::PLANT];
					assert(parent != INVALID_ENTITY_ID);
					const Vector2i& parentPos = m_ParentWorld->GetComponent<GridTransformComponent>(parent).m_Pos;
					Species parentSpecies = m_ParentWorld->GetComponent<SpeciesComponent>(parent).m_Species;
					assert(parentSpecies == Species::PLANT);

					const EntityID newborn = BlockoLifeWorldBuilder::BuildEntity(*m_ParentWorld, parentSpecies, {x, y});
					assert(newborn != INVALID_ENTITY_ID);

					DEBUG_PRINT("Entity %u (species=%d) at (%d, %d) birthed new entity %u at (%d, %d)\n",
						parent, parentSpecies, parentPos.x, parentPos.y, newborn, x, y);
				}
			}
		}
	}

	// Death pass (separate for now, TODO could avoid this extra pass by just putting the info needed in the cache above)
	// TODO death from not eating already done above! Should be done in same pass but before move?
	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			CreatureCache& cache = cachedEntities.At(x, y);
			if (cache.IsAlive() && cache.species == Species::PLANT)
			{
				assert(cache.species < Species::SPECIES_COUNT);
				if (cache.neighbourHealthTotals[(int)Species::PLANT] < 2 ||
					cache.neighbourHealthTotals[(int)Species::PLANT] > 3)
				{
					entitiesToRemove.insert(cache.entityID);
					cache = CreatureCache();
				}
			}
		}
	}

	for (EntityID e : entitiesToRemove)
	{
		m_ParentWorld->DestroyEntity(e);
	}
	
	steady_clock::time_point finish = steady_clock::now();

	std::cout << "\t --> took " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "[µs]" << std::endl;
	//std::cout << "\t --> took " << std::chrono::duration_cast<std::chrono::nanoseconds> (finish - start).count() << "[ns]" << std::endl;
}