#include "GridSpriteRenderSystem.h"

#include "World.h"
#include "GridWorldComponent.h"
#include "GridTransformComponent.h"
#include "SpriteComponent.h"
#include "SDLRenderManager.h"
#include "Coordinates.h"

void GridSpriteRenderSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "GridSpriteRenderSystem Init MISSING render manager!");

	EntitySignature renderSignature;
	renderSignature.set((size_t)ComponentType::CT_GRIDTRANSFORM);
	renderSignature.set((size_t)ComponentType::CT_SPRITE);
	m_ParentWorld->SetSystemSignature<GridSpriteRenderSystem>(renderSignature);
	m_ParentWorld->SetSystemDebugSignature<GridSpriteRenderSystem>(renderSignature);
}

void GridSpriteRenderSystem::Render_Main()
{
	auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();

	for (EntityID e : mEntities)
	{
		auto& t = m_ParentWorld->GetComponent<GridTransformComponent>(e);
		auto& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		m_RenderMan->Draw(r.m_SpriteID, gridWorld.GridToScreen(t.m_Pos));
	}
}
