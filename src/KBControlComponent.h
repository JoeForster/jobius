#pragma once

#include "ComponentManager.h"

struct KBControlComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_KBCONTROL; }

	KBControlComponent() {}

	bool m_Jump = false;
};
