#include "SpriteRenderSystem.h"

#include "World.h"
#include "SDLRenderManager.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "GridWorldComponent.h"

void SpriteRenderSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "SpriteRenderSystem Init MISSING render manager!");

	EntitySignature renderSignature;
	renderSignature.set((size_t)ComponentType::CT_TRANSFORM);
	renderSignature.set((size_t)ComponentType::CT_SPRITE);
	m_ParentWorld->SetSystemSignature<SpriteRenderSystem>(renderSignature);
	m_ParentWorld->SetSystemDebugSignature<SpriteRenderSystem>(renderSignature);
}

void SpriteRenderSystem::Render_Main()
{
	const auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();

	for (EntityID e : mEntities)
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		SpriteComponent& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		const auto screenPos = gridWorld.WorldToScreen(*m_ParentWorld, t.m_Pos);
		m_RenderMan->DrawSprite(r.m_SpriteID, r.m_TextureAlpha, screenPos);
	}
}
