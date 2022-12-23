#include "PlayerControlSystem.h"

#include <format>
#include <string>

#include "World.h"
#include "WorldCoords.h"
#include "SDLRenderManager.h"

#include "KBInputComponent.h"
#include "TransformComponent.h"
#include "PadInputComponent.h"
#include "RigidBodyComponent.h"
#include "DebugTextComponent.h"
#include "GridWorldComponent.h"

void PlayerControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM); // TODO_DEBUG_DRAW this is only needed for debug...
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	EntitySignature dbgSignature = sysSignature;
	dbgSignature.set((size_t)ComponentType::CT_DEBUGTEXT);
	m_ParentWorld->SetSystemSignatures<PlayerControlSystem>(sysSignature, dbgSignature);


	auto& renderInit = static_cast<const RenderSystemInitialiser&>(initialiser);
	m_RenderMan = renderInit.m_RenderMan;

	assert(m_RenderMan != nullptr && "PlayerControlSystem Init MISSING render manager!");
}


void PlayerControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : GetEntities())
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
	for (EntityID e : GetEntities())
	{
		auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		auto& pad = m_ParentWorld->GetComponent<PadInputComponent>(e);
		auto& dt = m_ParentWorld->GetComponent<DebugTextComponent>(e);

		static Vector2i offset (0, 20); 
		const auto drawPos = WorldCoords::WorldToScreen(*m_ParentWorld, t.m_Pos);
		m_RenderMan->DrawText(
			std::format("{:.2f}, {:.2f}, {:.2f}, {:.2f}",
				pad.GetAxis(GAMEPAD_AXIS::AXIS_LS_X),
				pad.GetAxis(GAMEPAD_AXIS::AXIS_LS_Y),
				pad.GetAxis(GAMEPAD_AXIS::AXIS_RS_X),
				pad.GetAxis(GAMEPAD_AXIS::AXIS_RS_Y)).c_str(),
			dt.m_ResID, drawPos + offset);
	}
}