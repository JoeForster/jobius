#pragma once

#include "ComponentManager.h"

enum class KB_KEY
{
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE,

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
