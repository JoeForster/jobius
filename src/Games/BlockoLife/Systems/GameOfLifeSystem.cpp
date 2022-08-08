#include "GameOfLifeSystem.h"

#include <string>
#include <format>
#include <algorithm>

#include "Vector.h"

#include "SpriteComponent.h"
#include "GridTransformComponent.h"
#include "Components/SpeciesComponent.h"

#include "World.h"
#include "SDLRenderManager.h"
#include "Coordinates.h"

void GameOfLifeSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_SPRITE);
	sysSignature.set((size_t)ComponentType::CT_BL_SPECIES);
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

	T& At(int x, int y)
	{
		size_t index = (y+m_OffsetY)*m_Height + (x+m_OffsetX);
		assert(index < NumCells());
		return m_Array[index];
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
	EntityID parentEntityID = INVALID_ENTITY_ID;
	Species species = Species::SPECIES_COUNT;
	uint8_t numNeighbours = 0;
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

// NAIVE implementation of Conway's Game of Life in ECS (but not really in ECS)
// TODO needs heavy tidy-up
void GameOfLifeSystem::Tick()
{
	// Solution 1: an for each living creature.
	// TODO: Inefficient lookup for now, may need either a cache or a better query system
	// (we'd like to be able to index into components, or adjacent ones, I guess?)
	Rect2i limits ( {0, 0}, {0, 0} );

	if (mEntities.empty())
	{
		printf("TICK no entities!");
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

	printf("TICK limits min(%d %d) max(%d %d)\n", limits.min.x, limits.min.y, limits.max.x, limits.max.y);

	assert(limits.max.y > limits.min.y);
	assert(limits.max.x > limits.min.x);
	assert(limits.min.x < 0);
	assert(limits.min.y < 0);

	size_t num_rows = (size_t)(limits.max.y - limits.min.y) + 1;
	size_t num_columns = (size_t)(limits.max.x - limits.min.x) + 1;
	size_t num_cells = num_rows * num_columns;	
	size_t x_offset = (size_t)(0 - limits.min.x);
	size_t y_offset = (size_t)(0 - limits.min.y);

	// TODO 2D array type or cache grid class here depending on what we do with component structure
	auto cachedEntities = Array2D<CreatureCache>(num_columns, num_rows, x_offset, y_offset);
	
	//EntityID* cachedEntities = new EntityID[num_cells];
	//for (int i = 0; i < num_cells; i++) cachedEntities[i] = INVALID_ENTITY_ID;

	auto getSpeciesAt = [&](int x, int y, EntityID& firstEntityFound) -> Species
	{
		CreatureCache& cache = cachedEntities.At(x, y);
		EntityID e = cache.entityID;
		if (e != INVALID_ENTITY_ID)
		{
			if (firstEntityFound == INVALID_ENTITY_ID) firstEntityFound = e;
			return cache.species;
		}
		else
		{
			return Species::SPECIES_COUNT;
		}
	};

	for (EntityID e : mEntities)
	{
		auto& transform = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		auto& species = m_ParentWorld->GetComponent<SpeciesComponent>(e);

		size_t index = transform.m_Pos.y*num_rows + transform.m_Pos.x;
		CreatureCache& cache = cachedEntities.At(transform.m_Pos.x, transform.m_Pos.y);
		assert(cache.entityID == INVALID_ENTITY_ID);
		cache.entityID = e;
		cache.species = species.m_Species;
	}

	// TODO parentSprite is HACK find better way! Ideally a basic archetype/creation system based on creature type
	auto countNeighbours = [&](int x, int y, Species s, EntityID& firstEntityFound) {
		uint8_t n = 0;

		if (getSpeciesAt(x-1, y-1, firstEntityFound) == s) ++n;
		if (getSpeciesAt(x,   y-1, firstEntityFound) == s) ++n;
		if (getSpeciesAt(x+1, y-1, firstEntityFound) == s) ++n;

		if (getSpeciesAt(x-1, y  , firstEntityFound) == s) ++n;
		if (getSpeciesAt(x+1, y  , firstEntityFound) == s) ++n;

		if (getSpeciesAt(x-1, y+1, firstEntityFound) == s) ++n;
		if (getSpeciesAt(x,   y+1, firstEntityFound) == s) ++n;
		if (getSpeciesAt(x+1, y+1, firstEntityFound) == s) ++n;

		return n;
	};

	//auto neighbourCounts = Array2D<uint8_t>(num_columns, num_rows, x_offset, y_offset, 0);
	// NOTE we leave out the "border areas" here as they have been added above and are blank.. This could be clearer.
	for (int y = limits.min.y+1; y < limits.max.y-1; ++y)
	{
		for (int x = limits.min.x+1; x < limits.max.x-1; ++x)
		{
			EntityID thisEntity = INVALID_ENTITY_ID;
			Species creature = getSpeciesAt(x, y, thisEntity);
			// HACK TODO filter by tag or different component type for animals that don't uset his logic?
			if (thisEntity != INVALID_ENTITY_ID && creature != Species::PLANT)
			{
				continue; 
			}

			EntityID parentEntity = INVALID_ENTITY_ID;
			uint8_t numNeighbours = countNeighbours(x, y, Species::PLANT, parentEntity);
			printf("neighbours at (%d, %d): %d\n", x, y, numNeighbours);
			
			CreatureCache& cache = cachedEntities.At(x, y);
			cache.species = creature;
			cache.numNeighbours = numNeighbours;
			cache.parentEntityID = parentEntity;
		}
	}

	// Move pass
	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			CreatureCache& cache = cachedEntities.At(x, y);
			if (cache.species == Species::HERBIVORE)
			{
				EntityID foundEntity = INVALID_ENTITY_ID;

				// TODO improve this, only one loop needed, could use helper?
				int plantScan[GridDir::COUNT];
				plantScan[GridDir::N] = countNeighbours(x, y-1, Species::PLANT, foundEntity); 
				plantScan[GridDir::NE] = countNeighbours(x+1, y-1, Species::PLANT, foundEntity); 
				plantScan[GridDir::E] = countNeighbours(x+1, y, Species::PLANT, foundEntity); 
				plantScan[GridDir::SE] = countNeighbours(x+1, y+1, Species::PLANT, foundEntity); 
				plantScan[GridDir::S] = countNeighbours(x, y+1, Species::PLANT, foundEntity); 
				plantScan[GridDir::SW] = countNeighbours(x-1, y+1, Species::PLANT, foundEntity); 
				plantScan[GridDir::W] = countNeighbours(x-1, y, Species::PLANT, foundEntity); 
				plantScan[GridDir::NW] = countNeighbours(x-1, y-1, Species::PLANT, foundEntity); 

				int mostPlants = 0;
				GridDir bestDir = (GridDir)0;
				for (int d = 0; d < GridDir::COUNT; ++d)
				{
					int numPlants = plantScan[d];
					if (numPlants > mostPlants)
					{
						mostPlants = numPlants;
						bestDir = (GridDir)d;
					}
				}

				if (mostPlants == 0)
				{
					bestDir = (GridDir)(rand() % GridDir::COUNT);
					printf("Entity %d at (%d, %d) Pick random dir: %d\n", foundEntity, x, y, (int)bestDir);
 				}
				else
				{
					printf("Entity %d at (%d, %d) Pick best dir %d\n", foundEntity, x, y, (int)bestDir);
				}
			}
		}
	}


	// Reproduction pass
	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			CreatureCache& cache = cachedEntities.At(x, y);
			bool isAlive = cache.entityID != INVALID_ENTITY_ID;
			if (!isAlive)
			{
				assert(cache.species == Species::SPECIES_COUNT);
				if (cache.numNeighbours == 3)
				{
					// TODO this duplicates creation logic in BlockoLifeWorldBuilder.cpp
					assert(cache.parentEntityID != INVALID_ENTITY_ID);
					ResourceID parentSprite = m_ParentWorld->GetComponent<SpriteComponent>(cache.parentEntityID).m_SpriteID;
					assert(parentSprite != ResourceID_Invalid);
					Species parentSpecies = m_ParentWorld->GetComponent<SpeciesComponent>(cache.parentEntityID).m_Species;

					EntityID newborn = m_ParentWorld->CreateEntity();
					const GridTransformComponent t(Vector2i(x, y));
					printf("Create at (%d, %d) -> entityID %u\n", x, y, newborn);
					m_ParentWorld->AddComponent<GridTransformComponent>(newborn, t);
					m_ParentWorld->AddComponent<SpriteComponent>(newborn, parentSprite);
					m_ParentWorld->AddComponent<SpeciesComponent>(newborn, parentSpecies);
				}
			}
		}
	}

	// Death pass (separate for now, TODO could avoid this extra pass by just putting the info needed in the cache above)
	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			CreatureCache& cache = cachedEntities.At(x, y);
			bool isAlive = cache.entityID != INVALID_ENTITY_ID;
			if (isAlive && cache.species == Species::PLANT)
			{
				assert(cache.species < Species::SPECIES_COUNT);
				if (cache.numNeighbours < 2)
				{
					m_ParentWorld->DestroyEntity(cache.entityID);
				}
				else if (cache.numNeighbours > 3)
				{
					m_ParentWorld->DestroyEntity(cache.entityID);
				}
			}
		}
	}
}