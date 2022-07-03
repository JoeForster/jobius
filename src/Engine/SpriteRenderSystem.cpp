#include "SpriteRenderSystem.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "World.h"

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
}

void SpriteRenderSystem::Render_Main()
{
	// TODO if multiple systems do render, then the "clear" stuff needs to be separated out?
	m_RenderMan->RenderClear();

	for (EntityID e : mEntities)
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		SpriteComponent& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		m_RenderMan->Draw(r.m_SpriteID, (int)t.m_Pos.x, (int)t.m_Pos.y);
	}
}
