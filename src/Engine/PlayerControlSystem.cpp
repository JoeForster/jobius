#include "PlayerControlSystem.h"

#include <sstream>

#include "KBInputComponent.h"
#include "TransformComponent.h"
#include "PadInputComponent.h"
#include "RigidBodyComponent.h"
#include "World.h"
#include "SDLRenderManager.h"

void PlayerControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM); // TODO_DEBUG_DRAW this is only needed for debug...
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<PlayerControlSystem>(sysSignature);


	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "BoxCollisionSystem Init MISSING render manager!");
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
		Vector2f desiredVel = Vector2f::ZERO;

		if (kbInput.IsPressed(KB_KEY::KEY_UP)) desiredVel.y -= 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_DOWN)) desiredVel.y += 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_LEFT)) desiredVel.x -= 1.0f;
		if (kbInput.IsPressed(KB_KEY::KEY_RIGHT)) desiredVel.x += 1.0f;

		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_UP)) desiredVel.y -= 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_DOWN)) desiredVel.y += 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_LEFT)) desiredVel.x -= 1.0f;
		if (padInput.IsPressed(GAMEPAD_BTN::BTN_DPAD_RIGHT)) desiredVel.x += 1.0f;

		desiredVel.x += padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_X);
		desiredVel.y += padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_Y);
		printf("DesiredVel: %f %f\n", desiredVel.x, desiredVel.y);

		rigidBody.m_Mass = kbInput.IsPressed(KB_KEY::KEY_SPACE) || padInput.IsPressed(GAMEPAD_BTN::BTN_FACE_CROSS) ? 0.0f : 1.0f;

		if (desiredVel.IsZero())
		{
			rigidBody.m_Kinematic = false;
		}
		else
		{
			rigidBody.m_Kinematic = true;
			rigidBody.m_Vel.x = padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_X);;// * 5.0f;
			rigidBody.m_Vel.y = padInput.GetAxis(GAMEPAD_AXIS::AXIS_LS_Y);;// * 5.0f;
		}
	}
}

void PlayerControlSystem::Render_Debug()
{
	for (EntityID e : mEntities)
	{
		//auto& kb = m_ParentWorld->GetComponent<KBInputComponent>(e);
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& pad = m_ParentWorld->GetComponent<PadInputComponent>(e);
		auto& rb = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		
		// TODO_DEBUG_DRAW here for now, but need a separate pass for debug.
		const ResourceID font = m_RenderMan->GetDefaultFont();
		
		// TODO tidy this, there has to be a better way of "find and update reference if it wasn't already in there"
		std::ostringstream ss;
		ss.precision(2);
		ss << rb.m_Vel.x << ", " << rb.m_Vel.y;

		EntityResMap::iterator existing = m_DebugText.find(e);
		if (existing == m_DebugText.end())
		{
			ResourceID text = ResourceID_Invalid;
			if (m_RenderMan->PrepareText(ss.str().c_str(), font, text))
			{
				m_RenderMan->DrawText(text, (int)t.m_Pos.x, (int)t.m_Pos.y);
				m_DebugText[e] = text;
			}
		}
		else
		{
			ResourceID text = existing->second;
			if (m_RenderMan->PrepareText(ss.str().c_str(), font, text))
			{
				m_RenderMan->DrawText(text, (int)t.m_Pos.x, (int)t.m_Pos.y);
			}
		}
	}
}