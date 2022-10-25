#pragma once

#include <memory>

#include "System.h"
#include "EntityManager.h"

class World;

class SystemManager
{
	// HACK need to refactor for entity list rebuilding!
	friend class World;

public:
	template<class T>
	std::shared_ptr<T> RegisterSystem(std::shared_ptr<World> parentWorld)
	{
		SystemType type = T::GetSystemType();
		size_t typeIndex = (size_t)type;
		assert(type < SystemType::ST_MAX);
		assert(m_Systems[typeIndex] == nullptr && "Registering system more than once.");
		auto system = std::make_shared<T>(parentWorld);
		m_Systems[typeIndex] = system;
		return system;
	}

	template<class T>
	void SetSignatures(EntitySignature signature, EntitySignature debugSignature)
	{
		SystemType type = T::GetSystemType();
		size_t typeIndex = (size_t)type;
		assert(m_Systems[typeIndex] != nullptr && "System used before registered.");

		m_Systems[typeIndex]->SetSignature(signature, debugSignature);
	}

	inline void SetEntitySetsDirty()
	{
		for (std::shared_ptr<System>& system : m_Systems)
		{
			if (system != nullptr)
			{
				system->SetEntitySetDirty();
			}
		}
	}

	void OnEntityDestroyed(EntityID entity)
	{
		// Erase a destroyed entity from all system lists
		for (std::shared_ptr<System>& system : m_Systems)
		{
			if (system != nullptr)
			{
				system->RemoveEntity(entity);
			}
		}
	}

	// TODO SLOW HERE!
	void UpdateEntitySets(EntityID entity, EntitySignature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (size_t systemIndex = 0; systemIndex < NUM_SYSTEM_TYPES; ++systemIndex)
		{
			System* system = m_Systems[systemIndex].get();
			if (system == nullptr)
			{
				continue; // This particular system was not created, which is fine
			}
		}
	}

	void UpdateAllSystems(float deltaSecs)
	{
		for (auto& system : m_Systems)
		{
			if (system != nullptr)
			{
				system->Update(deltaSecs);
			}
		}
	}

	void RenderAllSystems()
	{
		constexpr RenderPass renderPasses[] = { RenderPass::MAIN, RenderPass::DEBUG };
		for (auto pass : renderPasses)
		{
			for (auto& system : m_Systems)
			{
				if (system != nullptr)
				{
					system->Render(pass);
				}
			}
		}
	}



private:
	// WIP moving into System
	// Map from SystemType to a signature
	//EntitySignature m_Signatures[NUM_SYSTEM_TYPES];
	// Debug update may have a different signature for extra debug components
	//EntitySignature m_DebugSignatures[NUM_SYSTEM_TYPES];

	// Map from SystemType to a system pointer
	std::shared_ptr<System> m_Systems[NUM_SYSTEM_TYPES];
};