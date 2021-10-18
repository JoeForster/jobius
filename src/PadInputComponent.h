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

	BTN_FACE_Y = BTN_FACE_TRIANGLE,
	BTN_FACE_B = BTN_FACE_CIRCLE,
	BTN_FACE_A = BTN_FACE_CROSS,
	BTN_FACE_X = BTN_FACE_SQUARE,

	BTN_MAX
};

constexpr size_t NUM_GAMEPAD_BTNS = (size_t)GAMEPAD_BTN::BTN_MAX;

enum class GAMEPAD_AXIS
{
	AXIS_LS_X,
	AXIS_LS_Y,
	AXIS_RS_X,
	AXIS_RS_Y,

	AXIS_MAX
};

constexpr size_t NUM_GAMEPAD_AXES = (size_t)GAMEPAD_AXIS::AXIS_MAX;

struct PadInputComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_PADINPUT; }

	PadInputComponent() {}
	PadInputComponent(int padIndex): m_PadIndex(padIndex) {}

	int m_PadIndex = 0;
	std::bitset<NUM_GAMEPAD_BTNS> m_BtnState;
	float m_AxisState [NUM_GAMEPAD_AXES] = {};

	bool IsPressed(GAMEPAD_BTN btn) const { assert(btn < GAMEPAD_BTN::BTN_MAX); return m_BtnState[(size_t)btn]; }
	float GetAxis(GAMEPAD_AXIS axis) const { assert(axis < GAMEPAD_AXIS::AXIS_MAX); return m_AxisState[(size_t)axis]; }
};
