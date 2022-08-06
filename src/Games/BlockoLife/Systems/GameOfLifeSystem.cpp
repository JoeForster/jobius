#include "GameOfLifeSystem.h"

#include <string>
#include <format>

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

void GameOfLifeSystem::Tick()
{
	// Solution 1: an for each living creature.
	// TODO: Inefficient lookup for now, may need either a cache or a better query system
	// (we'd like to be able to index into components, or adjacent ones, I guess?)
	Rect2i limits ( {0, 0}, {0, 0} );

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

	size_t num_rows = (size_t)(limits.max.y - limits.min.y);
	size_t num_columns = (size_t)(limits.max.x - limits.min.x);
	size_t num_cells = num_rows * num_columns;	
	size_t x_offset = (size_t)(0 - limits.min.x);
	size_t y_offset = (size_t)(0 - limits.min.y);

	// TODO 2D array type or cache grid class here depending on what we do with component structure
	EntityID* cachedEntities = new EntityID[num_cells];
	for (int i = 0; i < num_cells; i++) cachedEntities[i] = INVALID_ENTITY_ID;

	
	auto getEntityAt = [&](int x, int y) -> EntityID& {
		int n = 0;
		size_t index = (y+y_offset)*num_rows + (x+x_offset);
		printf("Entity at (%d %d) index %zu\n", x, y, index);
		assert(index < num_cells);
		return cachedEntities[index];
	};
	auto getSpeciesAt = [&](int x, int y, EntityID& entityIDIfValid) -> Species {
		EntityID e = getEntityAt(x, y);
		if (e != INVALID_ENTITY_ID)
		{
			entityIDIfValid = e;
			auto s = m_ParentWorld->GetComponent<SpeciesComponent>(e);
			return s.m_Species;
		}
		else
		{
			return Species::SPECIES_COUNT;
		}
	};

	for (EntityID e : mEntities)
	{
		auto t = m_ParentWorld->GetComponent<GridTransformComponent>(e);

		size_t index = t.m_Pos.y*num_rows + t.m_Pos.x;
		EntityID& entityAtCoords = getEntityAt(t.m_Pos.x, t.m_Pos.y);
		assert(entityAtCoords == INVALID_ENTITY_ID);
		entityAtCoords = e;
	}

	// TODO parentSprite is HACK find better way! Ideally a basic archetype/creation system based on creature type
	auto countNeighbours = [&](int x, int y, Species s, EntityID& parentEntity) {
		int n = 0;
		if (getSpeciesAt(x-1, y-1, parentEntity) == s) ++n;
		if (getSpeciesAt(x,   y-1, parentEntity) == s) ++n;
		if (getSpeciesAt(x+1, y-1, parentEntity) == s) ++n;

		if (getSpeciesAt(x-1, y  , parentEntity) == s) ++n;
		if (getSpeciesAt(x+1, y  , parentEntity) == s) ++n;

		if (getSpeciesAt(x-1, y+1, parentEntity) == s) ++n;
		if (getSpeciesAt(x,   y+1, parentEntity) == s) ++n;
		if (getSpeciesAt(x+1, y+1, parentEntity) == s) ++n;

		return n;
	};

	for (int y = limits.min.y; y < limits.max.y; ++y)
	{
		for (int x = limits.min.x; x < limits.max.x; ++x)
		{
			EntityID thisEntity = INVALID_ENTITY_ID;
			Species creature = getSpeciesAt(x, y, thisEntity);
			if (creature != Species::PLANT) continue; // HACK TODO filter by tag or different component type for animals that don't uset his logic?
			assert(thisEntity != INVALID_ENTITY_ID);

			EntityID parentEntity = INVALID_ENTITY_ID;
			int numNeighbours = countNeighbours(x, y, creature, parentEntity);

			bool isAlive = creature != Species::SPECIES_COUNT;
			if (isAlive)
			{
				if (numNeighbours < 2)
				{
					m_ParentWorld->DestroyEntity(thisEntity);
				}
				else if (numNeighbours > 3)
				{
					m_ParentWorld->DestroyEntity(thisEntity);
				}
			}
			else
			{
				if (numNeighbours == 3)
				{
					assert(parentEntity != INVALID_ENTITY_ID);
					ResourceID parentSprite = m_ParentWorld->GetComponent<SpriteComponent>(parentEntity).m_SpriteID;

					// TODO this duplicates creation logic in BlockoLifeWorldBuilder.cpp
					EntityID newborn = m_ParentWorld->CreateEntity();
					const GridTransformComponent t(Vector2i(x, y));
					m_ParentWorld->AddComponent<GridTransformComponent>(newborn, t);
					m_ParentWorld->AddComponent<SpriteComponent>(newborn, parentSprite);
					m_ParentWorld->AddComponent<SpeciesComponent>(newborn, creature);
				}
			}
		}
	}

	delete[] cachedEntities;
}