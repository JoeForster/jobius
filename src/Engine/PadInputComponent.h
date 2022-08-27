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

	int m_PadIndex = -1;
	std::bitset<NUM_GAMEPAD_BTNS> m_PrevBtnState;
	std::bitset<NUM_GAMEPAD_BTNS> m_CurrBtnState;
	float m_AxisState [NUM_GAMEPAD_AXES] = {};

	bool IsValid() const { return (m_PadIndex >= 0); }
	bool WasJustPressed(GAMEPAD_BTN btn) const { assert(btn < GAMEPAD_BTN::BTN_MAX); return m_CurrBtnState[(size_t)btn] && !m_PrevBtnState[(size_t)btn]; }
	bool WasJustReleased(GAMEPAD_BTN btn) const { assert(btn < GAMEPAD_BTN::BTN_MAX); return !m_CurrBtnState[(size_t)btn] && m_PrevBtnState[(size_t)btn]; }
	bool IsPressed(GAMEPAD_BTN btn) const { assert(btn < GAMEPAD_BTN::BTN_MAX); return m_CurrBtnState[(size_t)btn]; }
	float GetAxis(GAMEPAD_AXIS axis) const { assert(axis < GAMEPAD_AXIS::AXIS_MAX); return m_AxisState[(size_t)axis]; }

	bool WasJustPressed(KB_KEY key) const { assert(key < KB_KEY::KEY_MAX); return m_CurrBtnState[(size_t)key] && !m_PrevBtnState[(size_t)key]; }
	bool WasJustReleased(KB_KEY key) const { assert(key < KB_KEY::KEY_MAX); return !m_CurrBtnState[(size_t)key] && m_PrevBtnState[(size_t)key]; }
	bool IsPressed(KB_KEY key) const { assert(key < KB_KEY::KEY_MAX); return m_CurrBtnState[(size_t)key]; }
};
