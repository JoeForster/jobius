#include "SpriteRenderSystem.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "World.h"
#include "SDLRenderManager.h"
#include "Coordinates.h"

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
	for (EntityID e : mEntities)
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		SpriteComponent& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		m_RenderMan->Draw(r.m_SpriteID, WorldToScreen(t.m_Pos));
	}
}
