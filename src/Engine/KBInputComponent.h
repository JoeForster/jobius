#pragma once

#include "ComponentManager.h"

enum class KB_KEY
{
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE,

	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_MAX
};

constexpr size_t NUM_KB_KEYS = (size_t)KB_KEY::KEY_MAX;

struct KBInputComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_KBINPUT; }

	KBInputComponent() {}

	std::bitset<NUM_KB_KEYS> m_State;

	bool IsPressed(KB_KEY key) const { assert(key < KB_KEY::KEY_MAX); return m_State[(size_t)key]; }
};
