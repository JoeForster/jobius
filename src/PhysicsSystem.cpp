#include "PhysicsSystem.h"

#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "World.h"

void PhysicsSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	sysSignature.set((size_t)ComponentType::CT_AABB);
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
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		auto& aabb = m_ParentWorld->GetComponent<AABBComponent>(e);

		// TODO Collision in separate system? Would need a way to queue and pass on the collision event/state
		bool colliding = false;
		for (EntityID testCollide : mEntities)
		{
			if (testCollide == e)
			{
				continue;
			}
			auto& transformTest = m_ParentWorld->GetComponent<TransformComponent>(testCollide);
			auto& aabbTest = m_ParentWorld->GetComponent<AABBComponent>(testCollide);
			if ( (fabsf(transformTest.m_Pos.x-t.m_Pos.x) < aabb.m_Box.x/2 + aabbTest.m_Box.x/2 ) &&
				(fabsf(transformTest.m_Pos.y-t.m_Pos.y) < aabb.m_Box.y/2 + aabbTest.m_Box.y/2) )
			{
				colliding = true;
			}
		}
		// TODO resolve collisions properly
		if (!colliding && !rb.m_Kinematic)
		{
			rb.m_Vel.y += gravityAccel * deltaSecs;
		}
		rb.m_Vel.LimitMagnitude(terminalVelocity);

		t.m_Pos.x += rb.m_Vel.x;
		t.m_Pos.y += rb.m_Vel.y;

		if (colliding)
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
