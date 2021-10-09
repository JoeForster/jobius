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

struct KBControlComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_KBCONTROL; }

	KBControlComponent() {}

	std::bitset<NUM_KB_KEYS> m_State;
};
