#pragma once

#include "ComponentManager.h"
#include "RenderTypes.h"

struct DebugTextComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_DEBUGTEXT; }

	DebugTextComponent()
		: m_ResID{ ResourceID_Invalid } {}

	DebugTextComponent(ResourceID resID)
		: m_ResID(resID) {}

	ResourceID m_ResID;
};
