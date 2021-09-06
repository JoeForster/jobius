#include "PhysicsSystem.h"

#include "Transform.h"
#include "World.h"

void PhysicsSystem::Update(float deltaSecs)
{
	// Hack test motion
	for (EntityID e : mEntities)
	{
		Transform& t = m_ParentWorld->GetComponent<Transform>(e);
		t.m_Pos.x = (float)(((int)t.m_Pos.x + 1) % 100);
		t.m_Pos.y = (float)(((int)t.m_Pos.y + 1) % 100);
	}
}