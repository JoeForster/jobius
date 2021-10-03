#include "PhysicsSystem.h"

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

	// Hack test motion
	for (EntityID e : mEntities)
	{
		TransformComponent& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		//t.m_Pos.x = (float)(((int)t.m_Pos.x + 1) % 100);
		t.m_Pos.y = (float)(((int)t.m_Pos.y + 1) % 100);
	}
}