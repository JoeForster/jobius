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
	m_ParentWorld->SetSystemSignatures<PlaneCollisionSystem>(sysSignature);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "PlaneCollisionSystem Init MISSING render manager!");
}


void PlaneCollisionSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	constexpr float gravityAccel = 9.8f;
	constexpr float terminalVelocity = 10.0f;
	for (EntityID e : GetEntities())
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& plane = m_ParentWorld->GetComponent<PlaneComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		bool colliding = false;
		for (EntityID testCollide : GetEntities())
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

// TODO_DEBUG_DRAW draw plane