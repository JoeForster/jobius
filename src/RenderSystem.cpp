#include "RenderSystem.h"

#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "World.h"


void RenderSystem::SetRenderManager(std::shared_ptr<SDLRenderManager> renderMan)
{
	assert(renderMan != nullptr);
	m_RenderMan = renderMan;
}

void RenderSystem::Init()
{
	System::Init();

	assert(m_RenderMan != nullptr && "RenderSystem Init MISSING render manager!");
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