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
	m_ParentWorld->SetSystemDebugSignature<SDLInputSystem>(sysSignature);

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

	constexpr Sint16 s_AxisDeadZone = 8000;
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
		auto& padControl = m_ParentWorld->GetComponent<PadInputComponent>(e);

		SDL_GameController* pad = nullptr;
		if (padControl.IsValid() && padControl.m_PadIndex < m_Controllers.size())
		{
			pad = m_Controllers[padControl.m_PadIndex];
		}
		if (pad != nullptr)
		{

			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_UP, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_DOWN, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_LEFT, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_DPAD_RIGHT, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_A, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_B, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_X, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X));
			padControl.m_BtnState.set((size_t)GAMEPAD_BTN::BTN_FACE_Y, SDL_GameControllerGetButton(pad, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y));

			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_LS_X] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_LS_Y] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);

			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_RS_X] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
			padControl.m_AxisState[(size_t)GAMEPAD_AXIS::AXIS_RS_Y] = ReadAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
		}

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		auto& kbControl = m_ParentWorld->GetComponent<KBInputComponent>(e);

		static constexpr SDL_Scancode KB_KEY_TO_SDL_SCANCODE[] =
		{
			SDL_SCANCODE_UP,		// KEY_UP
			SDL_SCANCODE_DOWN,		// KEY_DOWN
			SDL_SCANCODE_LEFT,		// KEY_LEFT
			SDL_SCANCODE_RIGHT,		// KEY_RIGHT
			SDL_SCANCODE_SPACE,		// KEY_SPACE

			SDL_SCANCODE_A,			// KEY_A
			SDL_SCANCODE_B,			// KEY_B
			SDL_SCANCODE_C,			// KEY_C
			SDL_SCANCODE_D,			// KEY_D
			SDL_SCANCODE_E,			// KEY_E
			SDL_SCANCODE_F,			// KEY_F
			SDL_SCANCODE_G,			// KEY_G
			SDL_SCANCODE_H,			// KEY_H
			SDL_SCANCODE_I,			// KEY_I
			SDL_SCANCODE_J,			// KEY_J
			SDL_SCANCODE_K,			// KEY_K
			SDL_SCANCODE_L,			// KEY_L
			SDL_SCANCODE_M,			// KEY_M
			SDL_SCANCODE_N,			// KEY_N
			SDL_SCANCODE_O,			// KEY_O
			SDL_SCANCODE_P,			// KEY_P
			SDL_SCANCODE_Q,			// KEY_Q
			SDL_SCANCODE_R,			// KEY_R
			SDL_SCANCODE_S,			// KEY_S
			SDL_SCANCODE_T,			// KEY_T
			SDL_SCANCODE_U,			// KEY_U
			SDL_SCANCODE_V,			// KEY_V
			SDL_SCANCODE_W,			// KEY_W
			SDL_SCANCODE_X,			// KEY_X
			SDL_SCANCODE_Y,			// KEY_Y
			SDL_SCANCODE_Z,			// KEY_Z
		};

		for (size_t key = 0; key < (size_t)KB_KEY::KEY_MAX; ++key)
		{
			kbControl.m_State.set((size_t)key, (bool)keystate[KB_KEY_TO_SDL_SCANCODE[key]]);
		}
	}
}