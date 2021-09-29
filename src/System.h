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

struct SystemInitialiser
{
};

class System
{
public:
	System(std::shared_ptr<World> parentWorld)
	: m_ParentWorld(parentWorld)
	, m_IsInited(false)
	{
		assert(m_ParentWorld != nullptr);
	}

	std::set<EntityID> mEntities;

	// Init to "finalise" the World, ready to render/update.
	virtual void Init(const SystemInitialiser& initialiser = s_EmptyInitialiser)
	{
		m_IsInited = true;
	}
	virtual void Update(float deltaSecs)
	{
		assert(IsInited() && "System updated before initialised");
	}
	virtual void Render()
	{
		assert(IsInited() && "System Render call before initialised");
	}

protected:
	static constexpr SystemInitialiser s_EmptyInitialiser{};

	std::shared_ptr<World> m_ParentWorld;

private:
	inline bool IsInited() const { return m_IsInited; }

	bool m_IsInited;
};
