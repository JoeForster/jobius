#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct RigidBodyComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_RIGIDBODY; }

	RigidBodyComponent() {}

	RigidBodyComponent(float mass)
		: m_Mass(mass) {}

	float m_Mass = 1.0f;
	Vector3f m_Vel = { 0.0f, 0.0f, 0.0f };
};
