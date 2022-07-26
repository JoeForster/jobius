#include "PlaneCollisionSystem.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "PlaneComponent.h"
#include "GridWorldComponent.h"

#include "World.h"
#include "SDLRenderManager.h"

void PlaneCollisionSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_PLANE);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PlaneCollisionSystem>(sysSignature);
	m_ParentWorld->SetSystemDebugSignature<PlaneCollisionSystem>(sysSignature);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "BoxCollisionSystem Init MISSING render manager!");
}


void PlaneCollisionSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	constexpr float gravityAccel = 9.8f;
	constexpr float terminalVelocity = 10.0f;
	for (EntityID e : mEntities)
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& plane = m_ParentWorld->GetComponent<PlaneComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		bool colliding = false;
		for (EntityID testCollide : mEntities)
		{
			if (testCollide == e)
			{
				continue;
			}
			auto& transformTest = m_ParentWorld->GetComponent<TransformComponent>(testCollide);
			//auto& aabbTest = m_ParentWorld->GetComponent<PlaneComponent>(testCollide);
			//if ( (fabsf((transformTest.m_Pos.x + aabb.m_Offset.x) - (t.m_Pos.x + aabbTest.m_Offset.x)) < aabb.m_Box.x/2 + aabbTest.m_Box.x/2 ) &&
			//	 (fabsf((transformTest.m_Pos.y + aabb.m_Offset.y) - (t.m_Pos.y + aabbTest.m_Offset.x)) < aabb.m_Box.y/2 + aabbTest.m_Box.y/2) )
			//{
			//	rb.m_Colliding = true;
			//	break;
			//}
		}
		// TODO resolve collisions properly (separate system even for this?)
	}
}

void PlaneCollisionSystem::Render_Debug()
{
	// TODO_DEBUG_DRAW move this to World render
	// TODO_DEBUG_DRAW support for colours
	// TODO_DEBUG_DRAW world->screen coordinates
	Rect2D bounds = m_ParentWorld->GetGlobalComponent<GridWorldComponent>().m_Bounds;

	m_RenderMan->DrawRect(bounds);

	for (EntityID e : mEntities)
	{
		// TODO_DEBUG_DRAW draw plane
		auto& transform = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& plane = m_ParentWorld->GetComponent<PlaneComponent>(e);
	}
}
