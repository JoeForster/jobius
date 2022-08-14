#include "CreatureSystem.h"

#include <string>
#include <format>

#include "Vector.h"

#include "SpriteComponent.h"
#include "GridTransformComponent.h"
#include "Components/SpeciesComponent.h"

#include "World.h"
#include "Coordinates.h"

void CreatureSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_SPRITE);
	sysSignature.set((size_t)ComponentType::CT_BL_SPECIES);
	sysSignature.set((size_t)ComponentType::CT_BL_HEALTH);
	m_ParentWorld->SetSystemSignature<CreatureSystem>(sysSignature);
	m_ParentWorld->SetSystemDebugSignature<CreatureSystem>(sysSignature);
}


void CreatureSystem::Update(float deltaSecs)
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

void CreatureSystem::Tick()
{
	// TODO needs behaviour moved out of GameOfLifeSystem
}