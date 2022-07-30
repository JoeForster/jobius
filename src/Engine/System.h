#pragma once

#include <set>
#include <memory>
#include <assert.h>

#include "RenderTypes.h"
#include "EntityManager.h"

enum class SystemType
{
	ST_RENDER,
	ST_GRIDRENDER,
	ST_BOXCOLLISION,
	ST_PLANECOLLISION,
	ST_PHYSICS,
	ST_INPUT,
	ST_PLAYERCONTROL,
	ST_NPCCONTROL,
	ST_NPCSENSOR,
	ST_MAX
};

constexpr size_t NUM_SYSTEM_TYPES = (size_t)SystemType::ST_MAX;

class World;

struct SystemInitialiser
{
};

// TODO remove SDL dependency hack here.
class SDLRenderManager;

struct RenderSystemInitialiser : public SystemInitialiser
{
	std::shared_ptr<SDLRenderManager> m_RenderMan;
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
	std::set<EntityID> mEntitiesDebug;

	// Init to "finalise" the World, ready to render/update.
	virtual void Init(const SystemInitialiser& initialiser = s_EmptyInitialiser)
	{
		m_IsInited = true;
	}
	virtual void Update(float deltaSecs)
	{
		assert(IsInited() && "System updated before initialised");
	}
	void Render(RenderPass pass)
	{
		assert(IsInited() && "System Render call before initialised");
		switch (pass)
		{
			case RenderPass::MAIN:
				Render_Main();
				break;
			case RenderPass::DEBUG:
				Render_Debug();
				break;
		}
	}

protected:
	static constexpr SystemInitialiser s_EmptyInitialiser{};

	std::shared_ptr<World> m_ParentWorld;

	// TODO template function that takes a RenderPass compile-time value?
	virtual void Render_Main() {}
	virtual void Render_Debug() {}

private:
	inline bool IsInited() const { return m_IsInited; }

	bool m_IsInited;
};
