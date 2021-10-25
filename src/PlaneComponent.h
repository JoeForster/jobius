#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct PlaneComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_PLANE; }

	PlaneComponent() {}

	PlaneComponent(float gradient, float intercept)
		: m_Gradient(gradient), m_Intercept(intercept) {}

	float m_Gradient = 0.f;
	float m_Intercept = 0.f;
};
