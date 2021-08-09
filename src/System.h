#pragma once

#include <set>

#include "EntityManager.h"

enum class SystemType
{
	ST_RENDER,
	ST_MAX
};

class System
{
public:
	std::set<EntityID> mEntities;

	virtual SystemType GetSystemType() const = 0;

	//virtual void Init() = 0;
	//virtual void Update(float deltaMs) = 0;
	//virtual void Render() = 0;
	//virtual void Destroy() = 0;

};