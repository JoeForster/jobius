#pragma once

#include <memory>

#include "System.h"
#include "EntityManager.h"

class World;

class SystemManager
{
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
	void SetSignature(EntitySignature signature)
	{
		SystemType type = T::GetSystemType();
		size_t typeIndex = (size_t)type;
		assert(m_Systems[typeIndex] != nullptr && "System used before registered.");

		m_Signatures[typeIndex] = signature;
	}

	template<class T>
	void SetDebugSignature(EntitySignature signature)
	{
		SystemType type = T::GetSystemType();
		size_t typeIndex = (size_t)type;
		assert(m_Systems[typeIndex] != nullptr && "System used before registered.");

		m_DebugSignatures[typeIndex] = signature;
	}

	void OnEntityDestroyed(EntityID entity)
	{
		// Erase a destroyed entity from all system lists
		for (std::shared_ptr<System>& system : m_Systems)
		{
			// TODO if we decide to guarantee all systems are initialised we could assert here
			if (system != nullptr)
			{
				// mEntities is a set so no check needed
				system->mEntities.erase(entity);
				system->mEntitiesDebug.erase(entity);
			}
		}
	}

	void OnEntitySignatureChanged(EntityID entity, EntitySignature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (size_t systemIndex = 0; systemIndex < NUM_SYSTEM_TYPES; ++systemIndex)
		{
			std::shared_ptr<System> system = m_Systems[systemIndex];
			if (system == nullptr)
			{
				continue; // This particular system was not created, which is fine
			}
			const EntitySignature& systemSignature = m_Signatures[systemIndex];

			// TODO use EntityQuery (or ultimately do away with signatures once we have a proper query system)
			// TODO following from above, shouldn't be necessary to have a second hard-coded signature for debug
			// - instead just tie a query to an update function (or allow multiple queries per system?)

			// Entity signature matches system signature - insert into set
			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->mEntities.insert(entity);
			}
			// Entity signature does not match system signature - erase from set
			else
			{
				system->mEntities.erase(entity);
			}

			// Same again for debug..
			const EntitySignature& debugSignature = m_DebugSignatures[systemIndex];
			if ((entitySignature & debugSignature) == debugSignature)
			{
				system->mEntitiesDebug.insert(entity);
			}
			else
			{
				system->mEntitiesDebug.erase(entity);
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
	// Map from SystemType to a signature
	EntitySignature m_Signatures[NUM_SYSTEM_TYPES];
	// Debug update may have a different signature for extra debug components
	EntitySignature m_DebugSignatures[NUM_SYSTEM_TYPES];

	// Map from SystemType to a system pointer
	std::shared_ptr<System> m_Systems[NUM_SYSTEM_TYPES];
};