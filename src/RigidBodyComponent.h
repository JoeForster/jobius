#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct RigidBodyComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_RIGIDBODY; }

	RigidBodyComponent() {}

	RigidBodyComponent(float mass)
		: m_Mass(mass) {}

	bool m_Kinematic = false;
	float m_Mass = 1.0f;
	Vector2f m_Vel = VECTOR2F_ZERO;
	Vector2f m_LastFrameForce = VECTOR2F_ZERO;
};
