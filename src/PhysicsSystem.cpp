#include "PhysicsSystem.h"

#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "World.h"

void PhysicsSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PhysicsSystem>(sysSignature);
}


void PhysicsSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// Hack test motion - TODO simple integrate here use mass properly
	constexpr float gravityAccel = 9.8f;
	constexpr float terminalVelocity = 10.0f;
	for (EntityID e : mEntities)
	{
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		if (!rb.m_Kinematic)
		{
			rb.m_Vel.y += gravityAccel * deltaSecs;
			rb.m_Vel.LimitMagnitude(terminalVelocity);
		}

		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		t.m_Pos.x += rb.m_Vel.x;
		t.m_Pos.y += rb.m_Vel.y;
	
		// HACK TEST until we have a floor: wrap around.
		if (t.m_Pos.y > 500)
		{
			t.m_Pos.y -= 500;
		}
	}
}