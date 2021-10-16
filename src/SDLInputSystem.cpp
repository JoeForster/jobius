#include "SDLInputSystem.h"

#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "World.h"

#include "SDL.h"


void SDLInputSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	m_ParentWorld->SetSystemSignature<SDLInputSystem>(sysSignature);

	int numJoysticks = SDL_NumJoysticks();
	printf("[SDLInputSystem] Num joysticks: %d\n", numJoysticks);
	m_Controllers.resize(numJoysticks);
	for (int ixJoystick = 0; ixJoystick < SDL_NumJoysticks(); ++ixJoystick)
	{
		m_Controllers[ixJoystick] = SDL_GameControllerOpen(ixJoystick);
	}
}

void SDLInputSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	constexpr Sint16 s_AxisDeadZone = 2000;
	constexpr Sint16 s_AxisHighVal = 32767;
	constexpr Sint16 s_AxisLowVal = -32768;
	auto ReadAxis = [&](SDL_GameControllerAxis axis)
	{
		Sint16 axisVal = SDL_GameControllerGetAxis(this->m_Controllers[0], axis);
		//printf("axis_%d = %d\n", axis, axisVal);
		float normalisedVal;
		if (axisVal >= s_AxisDeadZone)
		{
			assert(axisVal <= s_AxisHighVal);
			normalisedVal = (float)axisVal / (float)s_AxisHighVal;
		}
		else if (axisVal <= -s_AxisDeadZone)
		{
			assert(axisVal >= s_AxisLowVal);
			normalisedVal = (float)axisVal / (float)-s_AxisLowVal;
		}
		else
		{
			normalisedVal = 0.0f;
		}
		return normalisedVal;
	};

	for (EntityID e : mEntities)
	{
		if (m_Controllers[0] != nullptr)
		{
			auto& padControl = m_ParentWorld->GetComponent<PadInputComponent>(e);

			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_UP, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_DOWN, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_LEFT, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_RIGHT, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_A, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_B, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_X, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_Y, SDL_GameControllerGetButton(m_Controllers[0], SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y));

			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_LS_X] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_LS_Y] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);

			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_RS_X] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_RS_Y] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		auto& kbControl = m_ParentWorld->GetComponent<KBInputComponent>(e);

		kbControl.m_State.set((size_t)KB_KEY::KEY_UP, (bool)keystate[SDL_SCANCODE_UP]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_DOWN, (bool)keystate[SDL_SCANCODE_DOWN]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_LEFT, (bool)keystate[SDL_SCANCODE_LEFT]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_RIGHT, (bool)keystate[SDL_SCANCODE_RIGHT]);
	}
}