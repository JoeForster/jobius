#include "PlayerControlSystem.h"

#include "KBControlComponent.h"
#include "RigidBodyComponent.h"
#include "World.h"

void PlayerControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBCONTROL);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PlayerControlSystem>(sysSignature);
}


void PlayerControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : mEntities)
	{
		auto& kbControl = m_ParentWorld->GetComponent<KBControlComponent>(e);
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		// HACK TEST
		rigidBody.m_Vel.x = 0.0f;
		rigidBody.m_Vel.y = 0.0f;
		if (kbControl.m_State[(size_t)KB_KEY::KEY_UP]) rigidBody.m_Vel.y -= 1.0f;
		if (kbControl.m_State[(size_t)KB_KEY::KEY_DOWN]) rigidBody.m_Vel.y += 1.0f;
		if (kbControl.m_State[(size_t)KB_KEY::KEY_LEFT]) rigidBody.m_Vel.x -= 1.0f;
		if (kbControl.m_State[(size_t)KB_KEY::KEY_RIGHT]) rigidBody.m_Vel.x += 1.0f;

		rigidBody.m_Mass = kbControl.m_State[(size_t)KB_KEY::KEY_SPACE] ? 0.0f : 1.0f;
	}

}