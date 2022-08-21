#pragma once

#include "EntityManager.h"

struct HealthComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_HEALTH; }

	HealthComponent() {}
	HealthComponent(int health): m_Health(health), m_MaxHealth(health) {}
	HealthComponent(int health, int maxHealth): m_Health(health), m_MaxHealth(maxHealth) {}

	int m_Health = 10;
	int m_MaxHealth = 10;
	
	void ModHealth(int amount)
	{
		if (m_Health + amount > m_MaxHealth)
			m_Health = m_MaxHealth;
		else if (m_Health + amount < 0)
			m_Health = 0;
		else
			m_Health = m_Health + amount;
	}

	float GetHealthProportion() const
	{
		assert(m_Health > 0 && m_Health <= m_MaxHealth);
		return (float)m_Health / (float)m_MaxHealth;
	}
};
