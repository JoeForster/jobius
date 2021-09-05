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

	void EntityDestroyed(EntityID entity)
	{
		// Erase a destroyed entity from all system lists
		for (std::shared_ptr<System> system : m_Systems)
		{
			// TODO if we decide to guarantee all systems are initialised we could assert here
			if (system != nullptr)
			{
				// mEntities is a set so no check needed
				system->mEntities.erase(entity);
			}
		}
	}

	void EntitySignatureChanged(EntityID entity, EntitySignature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (size_t systemIndex = 0; systemIndex < NUM_SYSTEM_TYPES; ++systemIndex)
		{
			//SystemType type = (SystemType)systemIndex;
			std::shared_ptr<System> system = m_Systems[systemIndex];
			assert(system != nullptr);
			const EntitySignature& systemSignature = m_Signatures[systemIndex];

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
		}
	}

private:
	// Map from SystemType to a signature
	EntitySignature m_Signatures[NUM_SYSTEM_TYPES];

	// Map from SystemType to a system pointer
	std::shared_ptr<System> m_Systems[NUM_SYSTEM_TYPES];
};