#pragma once

#include "ComponentManager.h"

struct Vector3f
{
	float x = 0;
	float y = 0;
	float z = 0;
};

struct Transform
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_TRANSFORM; }

	Transform()
		: m_Pos{ 0, 0, 0 }, m_Rot{ 0, 0, 0 }, m_Scale{ 0, 0, 0 } {}

	Transform(Vector3f inPos, Vector3f inRot, Vector3f inScale)
		: m_Pos(inPos), m_Rot(inRot), m_Scale(inScale) {}

	Vector3f m_Pos;
	Vector3f m_Rot;
	Vector3f m_Scale;
};
