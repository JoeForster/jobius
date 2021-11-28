#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct RigidBodyComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_RIGIDBODY; }

	RigidBodyComponent() {}

	RigidBodyComponent(float mass)
		: m_Mass(mass) {}

	bool m_Colliding = false; // HACK - need proper collision resolution
	bool m_Kinematic = false;
	float m_Mass = 1.0f;
	Vector2f m_Vel = Vector2f::ZERO;
	Vector2f m_LastFrameForce = Vector2f::ZERO;
};
