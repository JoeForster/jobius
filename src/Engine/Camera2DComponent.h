#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct Camera2DComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_CAMERA_2D; }

	Camera2DComponent() {}

	// TODO proper transform matrix camera - unify with a 3D camera?
	Vector2f m_CameraPos = Vector2f::ZERO;
	float m_CameraZoom = 1.0f;

};
