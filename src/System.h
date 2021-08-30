#pragma once

#include <set>
#include <memory>
#include <assert.h>

#include "EntityManager.h"

enum class SystemType
{
	ST_RENDER,
	ST_PHYSICS,
	ST_MAX
};

constexpr size_t NUM_SYSTEM_TYPES = (size_t)SystemType::ST_MAX;

class World;

class System
{
public:
	System(std::shared_ptr<World> parentWorld)
	: m_ParentWorld(parentWorld)
	{
		assert(m_ParentWorld != nullptr);
	}

	std::set<EntityID> mEntities;

	virtual void Init() = 0;
	virtual void Update(float deltaSecs) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;

protected:
	std::shared_ptr<World> m_ParentWorld;

};