#pragma once

#include <memory>
#include <unordered_map>

#include "System.h"
#include "EntityManager.h"

class SystemManager
{
public:
	template<class T>
	std::shared_ptr<T> RegisterSystem()
	{
		auto system = std::make_shared<T>();
		SystemType type = system->GetSystemType();
		assert(type < SystemType::ST_MAX);
		assert(m_Systems.find(type) == m_Systems.end() && "Registering system more than once.");
		m_Systems.insert({ type, system });
		return system;
	}

	template<class T>
	void SetSignature(SystemType type, EntitySignature signature)
	{
		assert(m_Systems.find(type) != m_Systems.end() && "System used before registered.");

		m_Signatures.insert({ type, signature });
	}

	void EntityDestroyed(EntityID entity)
	{
		// Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : m_Systems)
		{
			auto const& system = pair.second;

			system->mEntities.erase(entity);
		}
	}

	void EntitySignatureChanged(EntityID entity, EntitySignature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : m_Systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = m_Signatures[type];

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
	// Map from system type string pointer to a signature
	std::unordered_map<SystemType, EntitySignature> m_Signatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<SystemType, std::shared_ptr<System>> m_Systems{};
};