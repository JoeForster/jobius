#include "PhysicsSystem.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "World.h"
#include "SDLRenderManager.h"

void PhysicsSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PhysicsSystem>(sysSignature);

	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "PhysicsSystem Init MISSING render manager!");
}


void PhysicsSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// Hack test motion - TODO simple integrate here use mass properly
	constexpr float gravityAccel = 9.8f;
	constexpr float terminalVelocity = 10.0f;
	for (EntityID e : mEntities)
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		if (!rb.m_Colliding && !rb.m_Kinematic)
		{
			rb.m_Vel.y += gravityAccel * deltaSecs;
		}
		rb.m_Vel.LimitMagnitude(terminalVelocity);

		t.m_Pos.x += rb.m_Vel.x;
		t.m_Pos.y += rb.m_Vel.y;

		if (rb.m_Colliding)
		{
			rb.m_Vel = VECTOR2F_ZERO;
		}
	
		// HACK TEST until we have a floor: wrap around.
		if (t.m_Pos.y > 500)
		{
			t.m_Pos.y -= 500;
		}
	}
}

void PhysicsSystem::Render()
{
	System::Render();

	for (EntityID e : mEntities)
	{
		//auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		
		// TODO debug draw entities
	}
}
