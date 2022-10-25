#include "CreatureSystem.h"

#include <string>
#include <format>

#include "Vector.h"
#include "SDLRenderManager.h"
#include "World.h"

#include "GridWorldComponent.h"
#include "GridTransformComponent.h"

#include "Components/SpeciesComponent.h"
#include "Components/HealthComponent.h"


void CreatureSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "CreatureSystem Init MISSING render manager!");
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_BL_SPECIES);
	sysSignature.set((size_t)ComponentType::CT_BL_HEALTH);
	m_ParentWorld->SetSystemSignatures<CreatureSystem>(sysSignature);
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

void CreatureSystem::Render_Main()
{
	const auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();

	for (EntityID e : GetEntities())
	{
		const auto& transform = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		const auto& species = m_ParentWorld->GetComponent<SpeciesComponent>(e);
		const auto& health = m_ParentWorld->GetComponent<HealthComponent>(e);
		if (species.m_Species == Species::PLANT)
		{
			continue;
		}

		//auto& health = m_ParentWorld->GetComponent<HealthComponent>(e);
		// TODO placeholder doesn't scale yet and needs render support for the filled rect call
		Vector2i screenPos = gridWorld.GridToScreen(*m_ParentWorld, transform.m_Pos);
		const int width = (int)(gridWorld.m_GridSize * health.GetHealthProportion());
		const Vector2i topLeft { screenPos.x, screenPos.y };
		const Vector2i bottomRight { screenPos.x+width, screenPos.y+4 };
		const Rect2i healthBar { topLeft, bottomRight };
		m_RenderMan->DrawFillRect(healthBar);
	}
}