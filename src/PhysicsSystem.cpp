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
	for (EntityID e : mEntities)
	{
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		//if (rb.m_Mass > 0)
		//{
		//	auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		//	t.m_Pos.x = (float)(((int)t.m_Pos.x + 1) % 100);
		//	t.m_Pos.y = (float)(((int)t.m_Pos.y + 1) % 100);
		//}

		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		t.m_Pos.x += rb.m_Vel.x;
		t.m_Pos.y += rb.m_Vel.y;
		t.m_Pos.z += rb.m_Vel.z;
	}
}