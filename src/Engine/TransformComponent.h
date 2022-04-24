#pragma once

#include "ComponentManager.h"

#include "Vector.h"

struct TransformComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_TRANSFORM; }

	TransformComponent()
		: m_Pos{ 0, 0, 0 }, m_Rot{ 0, 0, 0 }, m_Scale{ 0, 0, 0 } {}

	TransformComponent(Vector3f inPos, Vector3f inRot, Vector3f inScale)
		: m_Pos(inPos), m_Rot(inRot), m_Scale(inScale) {}

	Vector3f m_Pos;
	Vector3f m_Rot;
	Vector3f m_Scale;
};
