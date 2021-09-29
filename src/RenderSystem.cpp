#include "RenderSystem.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "World.h"

void RenderSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "RenderSystem Init MISSING render manager!");

	EntitySignature renderSignature;
	renderSignature &= (size_t)ComponentType::CT_TRANSFORM;
	renderSignature &= (size_t)ComponentType::CT_SPRITE;
	m_ParentWorld->SetSystemSignature<RenderSystem>(renderSignature);
}

void RenderSystem::Render()
{
	System::Render();

	m_RenderMan->RenderClear();

	for (EntityID e : mEntities)
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		SpriteComponent& r = m_ParentWorld->GetComponent<SpriteComponent>(e);
		m_RenderMan->Draw(r.m_SpriteID, (int)t.m_Pos.x, (int)t.m_Pos.y);
	}
}