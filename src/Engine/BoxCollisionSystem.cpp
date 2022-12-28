#include "BoxCollisionSystem.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "World.h"
#include "SDLRenderManager.h"
#include "WorldCoords.h"

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
			if (transformTest.m_Pos.x >= aabbTest.m_Box.min.x && transformTest.m_Pos.y >= aabbTest.m_Box.min.y &&
				transformTest.m_Pos.x <= aabbTest.m_Box.max.x && transformTest.m_Pos.y <= aabbTest.m_Box.min.y)
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
		m_RenderMan->DrawRect((Rect2i)aabb.m_Box, WorldCoords::WorldToScreen(*m_ParentWorld, t.m_Pos));
	}
}
