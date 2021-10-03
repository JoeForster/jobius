#pragma once

#include "ComponentManager.h"

struct RigidBodyComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_RIGIDBODY; }

	RigidBodyComponent() {}

	RigidBodyComponent(float mass)
		: m_Mass(mass) {}

	float m_Mass = 1.0f;
};
