#include "SpriteRenderSystem.h"

#include "World.h"
#include "SDLRenderManager.h"
#include "WorldCoords.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"

void SpriteRenderSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "SpriteRenderSystem Init MISSING render manager!");

	EntitySignature renderSignature;
	renderSignature.set((size_t)ComponentType::CT_TRANSFORM);
	renderSignature.set((size_t)ComponentType::CT_SPRITE);
	m_ParentWorld->SetSystemSignatures<SpriteRenderSystem>(renderSignature);
}

void SpriteRenderSystem::Render_Main()
{
	for (EntityID e : GetEntities())
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		SpriteComponent& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		const auto screenPos = WorldCoords::WorldToScreen(*m_ParentWorld, t.m_Pos);
		m_RenderMan->DrawSprite(r.m_SpriteID, r.m_TextureAlpha, screenPos);
	}
}
