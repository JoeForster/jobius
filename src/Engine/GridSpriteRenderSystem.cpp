#include "GridSpriteRenderSystem.h"

#include "World.h"
#include "GridWorldComponent.h"
#include "GridTransformComponent.h"
#include "SpriteComponent.h"
#include "SDLRenderManager.h"
#include "Coordinates.h"

#define DEBUG_DRAW_GRID_ENABLED 0

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

void GridSpriteRenderSystem::Render_Debug()
{
#if DEBUG_DRAW_GRID_ENABLED
	auto& gridWorld = m_ParentWorld->GetGlobalComponent<GridWorldComponent>();
	Rect2D bounds = gridWorld.m_Bounds;

	m_RenderMan->DrawRect(bounds);

	const Colour4i gridColour = { 127, 127, 127, 255 };

	for (float y = bounds.min.y + gridWorld.m_GridSize; y < bounds.max.y; y += gridWorld.m_GridSize) 
	{
		Vector3f start (bounds.min.x, y, 0.0f);
		Vector3f end (bounds.max.x, y, 0.0f);
		m_RenderMan->DrawLine(WorldToScreen(start), WorldToScreen(end), gridColour);
	}
	
	for (float x = bounds.min.x + gridWorld.m_GridSize; x < bounds.max.x; x += gridWorld.m_GridSize) 
	{
		Vector3f start (x, bounds.min.y, 0.0f);
		Vector3f end (x, bounds.max.y, 0.0f);
		m_RenderMan->DrawLine(WorldToScreen(start), WorldToScreen(end), gridColour);
	}
#endif // DEBUG_DRAW_GRID_ENABLED
}
