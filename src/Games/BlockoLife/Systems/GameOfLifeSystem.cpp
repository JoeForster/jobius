#include "GameOfLifeSystem.h"

#include <vector>
#include <string>
#include <format>
#include <algorithm>

#include "Vector.h"

#include "GridTransformComponent.h"
#include "Components/SpeciesComponent.h"
#include "Components/HealthComponent.h"

#include "World.h"
#include "SDLRenderManager.h"

#include "BlockoLifeWorldBuilder.h"

constexpr bool DESTROY_PLANT_ON_EATEN = false;

void GameOfLifeSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_SPRITE);
	sysSignature.set((size_t)ComponentType::CT_BL_SPECIES);
	sysSignature.set((size_t)ComponentType::CT_BL_HEALTH);
	m_ParentWorld->SetSystemSignature<GameOfLifeSystem>(sysSignature);
	m_ParentWorld->SetSystemDebugSignature<GameOfLifeSystem>(sysSignature);
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

// TEMP-ish structure until we decide properly how this data should be stored/cached.
template<typename T>
class Array2D
{
public:
	Array2D(size_t width, size_t height, size_t offsetX, size_t offsetY)//, const T& defaultVal)
	: m_Width(width)
	, m_Height(height)
	, m_OffsetX(offsetX)
	, m_OffsetY(offsetY)
	{
		assert(width > 0);
		assert(height > 0);
		// TODO: Compiler warning makes no sense?!
		m_Array = new T[NumCells()];
		//for (size_t i = 0; i < NumCells(); ++i) m_Array[i] = defaultVal;
	}
	~Array2D()
	{
		delete[] m_Array;
	}

    Array2D(const Array2D&) = delete;
    Array2D& operator=(const Array2D&) = delete;
    Array2D(Array2D&&) = delete;
    Array2D& operator=(Array2D&&) = delete;

	inline T& At(int x, int y)
	{
		size_t index = (y+m_OffsetY)*m_Width + (x+m_OffsetX);
		assert(index < NumCells());
		return m_Array[index];
	}
	inline T& At(const Vector2i& p)
	{
		return At(p.x, p.y);
	}

private:
	inline size_t NumCells() const { return m_Width * m_Height; }

	T* m_Array;
	size_t m_Width;
	size_t m_Height;
	size_t m_OffsetX;
	size_t m_OffsetY;

};

struct CreatureCache
{
	EntityID entityID = INVALID_ENTITY_ID;
	Species species = Species::SPECIES_COUNT;
	int health = 0;
	uint8_t neighbourHealthTotals[SpeciesCount] = { 0 };
	EntityID parentEntityIDs[(size_t)Species::SPECIES_COUNT] = { INVALID_ENTITY_ID }; // hacky, it's just the first entity found of each species if any

	bool IsAlive() const { return entityID != INVALID_ENTITY_ID && health > 0; }
};

enum GridDir
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
static_assert(GridDir::N == 0);
static_assert(GridDir::COUNT == 8);

static constexpr Vector2i GridDirToVec[GridDir::COUNT] = {
	{ 0, -1 },		// GridDir::N
	{ 1, -1 },		// GridDir::NE
	{ 1,  0 },		// GridDir::E
	{ 1,  1 },		// GridDir::SE
	{ 0,  1 },		// GridDir::S
	{-1,  1 },		// GridDir::SW
	{-1,  0 },		// GridDir::W
	{-1, -1 },		// GridDir::NW
};

// NAIVE implementation of Conway's Game of Life in ECS (but not really in ECS)
// TODO needs heavy tidy-up
void GameOfLifeSystem::Tick()
{
	// Solution 1: an for each living creature.
	// TODO: Inefficient lookup for now, may need either a cache or a better query system
	// (we'd like to be able to index into components, or adjacent ones, I guess?)
	Rect2i limits ( {0, 0}, {0, 0} );

	printf("\n\nTICK\n\n");

	if (mEntities.empty())
	{
		printf("TICK no entities!\n");
	}

	for (EntityID e : mEntities)
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

	printf("TICK entities(%zu) limits min(%d %d) max(%d %d)\n", mEntities.size(), limits.min.x, limits.min.y, limits.max.x, limits.max.y);

	assert(limits.max.y > limits.min.y);
	assert(limits.max.x > limits.min.x);
	assert(limits.min.x < 0);
	assert(limits.min.y < 0);

	size_t num_rows = (size_t)(limits.max.y - limits.min.y) + 1;
	size_t num_columns = (size_t)(limits.max.x - limits.min.x) + 1;
	size_t num_cells = num_rows * num_columns;	
	// TODO BUG HERE?
	size_t x_offset = (size_t)(0 - limits.min.x);
	size_t y_offset = (size_t)(0 - limits.min.y);

	// TODO we're kind of bypassing the ECS here with custom storage - we need a way of storing components
	// in the right order OR so we can index into adjacent cells!
	auto cachedEntities = Array2D<CreatureCache>(num_columns, num_rows, x_offset, y_offset);

	for (EntityID e : mEntities)
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

	// TODO parentSprite is HACK find better way! Ideally a basic archetype/creation system based on creature type
	auto countNeighbours = [&](int x, int y, Species s, EntityID& firstEntityFound) {
		uint8_t n = 0;
		for (const Vector2i& dirVec : GridDirToVec)
		{
			const Vector2i checkPos = Vector2i(x, y) + dirVec;
			CreatureCache& cacheAt = cachedEntities.At(checkPos);
			if (cacheAt.species == s)
			{
				if (firstEntityFound == INVALID_ENTITY_ID)
				{
					firstEntityFound = cacheAt.entityID;
				}
				++n;
			}
		}
		return n;
	};

	std::set<EntityID> entitiesToRemove;

	// Move pass
	for (EntityID movingEntity : mEntities)
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

		// HACK herbivore behaviour should be in its own system
		if (cache.species == Species::HERBIVORE)
		{
			printf("Move entity %d\n", movingEntity);
			auto& health = m_ParentWorld->GetComponent<HealthComponent>(movingEntity);

			GridDir possibleDirs[GridDir::COUNT];
			int numPossibleDirs = 0;

			bool didEat = false;

			int mostPlants = 0;
			GridDir bestDir = (GridDir)0;
			for (GridDir dir = (GridDir)0; dir < GridDir::COUNT; dir = (GridDir)( (int)dir+1 ))
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

						printf("Entity %d at (%d, %d) eat plant %d at (%d, %d)! Health %d -> %d\n",
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
					possibleDirs[numPossibleDirs++] = dir;
					EntityID foundEntity = INVALID_ENTITY_ID;
					int numPlants = countNeighbours(x, y-1, Species::PLANT, foundEntity);
					if (numPlants > mostPlants)
					{
						mostPlants = numPlants;
						bestDir = (GridDir)dir;
					}
				}
			}

			if (!didEat)
			{
				constexpr int healthLoss = -1;
				const int healthBefore = health.m_Health;
				health.ModHealth(healthLoss);
				const int healthAfter = health.m_Health;
				printf("Entity %d at (%d, %d) didn't eat - Health %d -> %d\n",
					movingEntity, x, y, healthBefore, healthAfter);
				if (healthAfter == 0)
				{
					printf("Entity %d at (%d, %d) didn't eat - ZERO HEALTH, STARVED!",
						movingEntity, x, y);
					entitiesToRemove.insert(movingEntity);
					cache = CreatureCache();
					continue;
				}
			}

			if (numPossibleDirs == 0)
			{
				printf("Entity %d at (%d, %d) is BLOCKED!\n", movingEntity, x, y);
				continue;
			}
			else if (mostPlants == 0)
			{
				const int bestDirIndex = rand() % numPossibleDirs;
				bestDir = possibleDirs[bestDirIndex];
				printf("Entity %d at (%d, %d) Pick random dir: %d\n", movingEntity, x, y, (int)bestDir);
 			}
			else
			{
				printf("Entity %d at (%d, %d) Pick best dir %d\n", movingEntity, x, y, (int)bestDir);
			}
				
			// TODO bug here - herbivores unexpectedly multiply (bad move?), and then cache position doesn't match!
			auto& thisTransform = m_ParentWorld->GetComponent<GridTransformComponent>(movingEntity);
			if (thisTransform.m_Pos.x != x || thisTransform.m_Pos.y != y)
			{
				printf("OOPS this object %d is at unexpected position - original (%d, %d) in cache (%d, %d)\n",
					movingEntity, thisTransform.m_Pos.x, thisTransform.m_Pos.y, x, y);
				assert(false);
			}
			const Vector2i moveIntoPos = thisTransform.m_Pos + GridDirToVec[bestDir];

			auto& target = cachedEntities.At(moveIntoPos.x, moveIntoPos.y);

			// UGH we have two places where this info lives due to the cache - this can be better
			// once we use the actual component storage rather than separate cache.
			//assert(!target.beingMovedInto);
			target = cache;
			cache = CreatureCache();
			thisTransform.m_Pos = moveIntoPos;

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
			EntityID parentEntity = INVALID_ENTITY_ID;
			uint8_t numPlantNeighbours = countNeighbours(x, y, Species::PLANT, parentEntity);
			//printf("neighbours at (%d, %d): %d\n", x, y, numNeighbours);
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

					printf("Entity %u (species=%d) at (%d, %d) birthed new entity %u at (%d, %d)\n",
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
}