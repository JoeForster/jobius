#include "NPCControlSystem.h"

#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "RigidBodyComponent.h"
#include "World.h"

void NPCControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_NPCBHV);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<NPCControlSystem>(sysSignature);
}


void NPCControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : mEntities)
	{
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		// TODO plug in behaviour tree actions/queries and test
		//Vector2f desiredVel;

		// HACK TEST movement just to prove this is plugged in correctly
		static float counter = 0;
		static float vel = 1.0f;

		counter += deltaSecs;
		if (counter >= 2.0f)
		{
			counter = 0.0f;
			vel *= -1.0f;
		}
		rigidBody.m_Vel.x = vel;

		//if (UP)) rigidBody.m_Vel.y -= 1.0f;
		//if (DOWN)) rigidBody.m_Vel.y += 1.0f;
		//if (LEFT)) rigidBody.m_Vel.x -= 1.0f;
		//if (RIGHT)) rigidBody.m_Vel.x += 1.0f;

		rigidBody.m_Mass = 0.0f;
		rigidBody.m_Kinematic = true;
	}
}
