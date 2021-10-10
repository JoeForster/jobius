#pragma once

#include "ComponentManager.h"

enum class GAMEPAD_BTN
{
	BTN_DPAD_UP,
	BTN_DPAD_DOWN,
	BTN_DPAD_LEFT,
	BTN_DPAD_RIGHT,

	BTN_FACE_TRIANGLE,
	BTN_FACE_CIRCLE,
	BTN_FACE_CROSS,
	BTN_FACE_SQUARE,

	BTN_MAX
};

constexpr size_t NUM_GAMEPAD_BTNS = (size_t)GAMEPAD_BTN::BTN_MAX;

struct PadInputComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_PADINPUT; }

	PadInputComponent() {}

	std::bitset<NUM_GAMEPAD_BTNS> m_State;

	bool IsPressed(GAMEPAD_BTN btn) const { assert(btn < GAMEPAD_BTN::BTN_MAX); return m_State[(size_t)btn]; }
};
