#pragma once

#include "EntityManager.h"

struct HealthComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_HEALTH; }

	HealthComponent() {}
	HealthComponent(int health): m_Health(health) {}

	int m_Health = 10;
};
