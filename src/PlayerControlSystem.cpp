#include "PlayerControlSystem.h"

#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "RigidBodyComponent.h"
#include "World.h"

void PlayerControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PlayerControlSystem>(sysSignature);
}


void PlayerControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : mEntities)
	{
		auto& kbInput = m_ParentWorld->GetComponent<KBInputComponent>(e);
		auto& padInput = m_ParentWorld->GetComponent<PadInputComponent>(e);
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		// HACK TEST
		Vector2f desiredVel;

		if (kbInput.IsPressed(KB_KEY::KEY_UP)) rigidBody.m_Vel.y -= 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_DOWN)) rigidBody.m_Vel.y += 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_LEFT)) rigidBody.m_Vel.x -= 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_RIGHT)) rigidBody.m_Vel.x += 1.0f;

		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_UP)) rigidBody.m_Vel.y -= 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_DOWN)) rigidBody.m_Vel.y += 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_LEFT)) rigidBody.m_Vel.x -= 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_RIGHT)) rigidBody.m_Vel.x += 1.0f;

		rigidBody.m_Vel.x += padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_X);
		rigidBody.m_Vel.y += padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_Y);

		rigidBody.m_Mass = kbInput.IsPressed(KB_KEY::KEY_SPACE) || padInput.IsPressed(GAMEPAD_BTN::BTN_FACE_CROSS) ? 0.0f : 1.0f;

		if (desiredVel.IsZero())
		{
			rigidBody.m_Kinematic = false;
		}
		else
		{
			rigidBody.m_Kinematic = true;
			rigidBody.m_Vel.x = padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_X);
			rigidBody.m_Vel.y = padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_Y);
		}
	}
}
