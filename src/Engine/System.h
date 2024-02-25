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
	ST_CAMERA_2D,
	ST_NPCCONTROL,
	ST_NPCSENSOR,
	ST_BEHAVIOUR,
	// TODO: Game-specific components below, requires a better system to keep them in game code
	ST_BL_GAMEOFLIFE,
	ST_BL_CREATURE,
	ST_BL_BLOCKDROPPER,
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
	, m_EntitiesDirty(true)
	{
		assert(m_ParentWorld != nullptr);
	}

	// TODO make private
	// TODO naming consistency m_!

	// Init to "finalise" the World, ready to render/update.
	virtual void Init(const SystemInitialiser& initialiser = s_EmptyInitialiser)
	{
		m_IsInited = true;
	}
	virtual void Update(float deltaSecs)
	{
		assert(IsInited() && "System updated before initialised");

		if (IsEntitySetDirty())
		{
			UpdateEntitySet();
		}
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

	void SetSignature(const EntitySignature& signature, const EntitySignature& dbgSignature)
	{
		if (m_Signature != signature || m_DebugSignature != dbgSignature)
		{
			m_Signature = signature;
			m_DebugSignature = dbgSignature;
			m_EntitiesDirty = true;
		}
	}

	const std::set<EntityID>& GetEntities() const
	{
		if (m_EntitiesDirty)
		{
			// TODO update here?
			assert(false && "Entity list needs updating");
		}
		return m_Entities;
	}

	const std::set<EntityID>& GetEntitiesDebug() const
	{
		if (m_EntitiesDirty)
		{
			// TODO update here?
			assert(false && "Entity list needs updating");
		}
		return m_EntitiesDebug;
	}

	void RemoveEntity(EntityID e)
	{
		m_Entities.erase(e);
		m_EntitiesDebug.erase(e);
	}

	inline bool IsEntitySetDirty() const
	{
		return m_EntitiesDirty;
	}

	inline void SetEntitySetDirty()
	{
		m_EntitiesDirty = true;
	}

	// Update our entities for a specific entity
	//void UpdateEntitySet(EntityID entity, EntitySignature entitySignature)
	//{
	//	// TODO unify this with EntityQuery so that a "system entity list" is just the (cached) result of one?
	//	// Could even have a query system deal with the caching so that the system doesn't need to keep these lists..?
	//	assert(!m_EntitiesDirty && "Can only UpdateEntitySet for one entity if list is already up-to-date");
	//
	//	// Entity signature matches system signature - insert into set
	//	if ((entitySignature & m_Signature) == m_Signature)
	//	{
	//		m_Entities.insert(entity);
	//	}
	//	// Entity signature does not match system signature - erase from set
	//	else
	//	{
	//		m_Entities.erase(entity);
	//	}
	//
	//	// Same again for debug..
	//	if ((entitySignature & m_DebugSignature) == m_DebugSignature)
	//	{
	//		m_EntitiesDebug.insert(entity);
	//	}
	//	else
	//	{
	//		m_EntitiesDebug.erase(entity);
	//	}
	//}

	void UpdateEntitySet();

protected:
	static constexpr SystemInitialiser s_EmptyInitialiser{};

	std::shared_ptr<World> m_ParentWorld;

	// TODO template function that takes a RenderPass compile-time value?
	virtual void Render_Main() {}
	virtual void Render_Debug() {}

private:
	inline bool IsInited() const { return m_IsInited; }
	
	bool m_IsInited;
	
	EntitySignature m_Signature;
	EntitySignature m_DebugSignature;

	// WIP: These are "redundant" in that they depend on the signatures above, but it's far too slow for large worlds
	// to be rebuilding these lists every time we add an entity. so implementing dirty flag and ability to
	// update either on access or at runtime (after simulation start) or in one go (at simulation start)
	std::set<EntityID> m_Entities;
	std::set<EntityID> m_EntitiesDebug;
	bool m_EntitiesDirty;

};
