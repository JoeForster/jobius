#pragma once

#include "ComponentManager.h"

struct PlayerComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_PLAYER; }

	PlayerComponent() {}
	PlayerComponent(int playerIndex): m_PlayerIndex(playerIndex) {}

	int m_PlayerIndex = -1;
};
