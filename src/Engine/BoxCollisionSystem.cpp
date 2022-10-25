#include "BoxCollisionSystem.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "World.h"
#include "SDLRenderManager.h"

void BoxCollisionSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_AABB);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignatures<BoxCollisionSystem>(sysSignature);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "BoxCollisionSystem Init MISSING render manager!");
}


void BoxCollisionSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	constexpr float gravityAccel = 9.8f;
	constexpr float terminalVelocity = 10.0f;
	for (EntityID e : GetEntities())
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& aabb = m_ParentWorld->GetComponent<AABBComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		rb.m_Colliding = false;
		for (EntityID testCollide : GetEntities())
		{
			if (testCollide == e)
			{
				continue;
			}
			auto& transformTest = m_ParentWorld->GetComponent<TransformComponent>(testCollide);
			auto& aabbTest = m_ParentWorld->GetComponent<AABBComponent>(testCollide);
			if ( (fabsf((transformTest.m_Pos.x + aabb.m_Offset.x) - (t.m_Pos.x + aabbTest.m_Offset.x)) < aabb.m_Box.x/2 + aabbTest.m_Box.x/2 ) &&
				 (fabsf((transformTest.m_Pos.y + aabb.m_Offset.y) - (t.m_Pos.y + aabbTest.m_Offset.x)) < aabb.m_Box.y/2 + aabbTest.m_Box.y/2) )
			{
				rb.m_Colliding = true;
				break;
			}
		}
		// TODO resolve collisions properly (separate system even for this?)
	}
}

void BoxCollisionSystem::Render_Debug()
{
	for (EntityID e : GetEntities())
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& aabb = m_ParentWorld->GetComponent<AABBComponent>(e);

		const int minX = (int) (t.m_Pos.x + aabb.m_Offset.x - aabb.m_Box.x/2);
		const int maxX = (int) (t.m_Pos.x + aabb.m_Offset.x + aabb.m_Box.x/2);
		const int minY = (int) (t.m_Pos.y + aabb.m_Offset.y - aabb.m_Box.y/2);
		const int maxY = (int) (t.m_Pos.y + aabb.m_Offset.y + aabb.m_Box.y/2);

		// TODO multi draw line
		m_RenderMan->DrawLine(minX, minY, maxX, minY);
		m_RenderMan->DrawLine(maxX, minY, maxX, maxY);
		m_RenderMan->DrawLine(maxX, maxY, minX, maxY);
		m_RenderMan->DrawLine(minX, maxY, minX, minY);
	}
}
