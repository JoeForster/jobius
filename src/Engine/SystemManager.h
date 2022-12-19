#pragma once

#include <memory>
#include <map>

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
		assert(type < SystemType::ST_MAX);
		assert(!m_Systems.contains[type] && "Registering system more than once.");
		auto system = std::make_shared<T>(parentWorld);
		m_Systems[type] = system;
		return system;
	}

	template<class T>
	void SetSignatures(EntitySignature signature, EntitySignature debugSignature)
	{
		SystemType type = T::GetSystemType();
		assert(m_Systems.contains(typeIndex) != nullptr && "System used before registered.");
		m_Systems[type]->SetSignature(signature, debugSignature);
	}

	inline void SetEntitySetsDirty()
	{
		for (auto& system : m_Systems)
		{
			system.second->SetEntitySetDirty();
		}
	}

	void OnEntityDestroyed(EntityID entity)
	{
		// Erase a destroyed entity from all system lists
		for (auto& system : m_Systems)
		{
			system.second->RemoveEntity(entity);
		}
	}

	// TODO is this needed? If so, restore it but with a different name
	//void UpdateEntitySets(EntityID entity, EntitySignature entitySignature)
	//{
	//	// Notify each system that an entity's signature changed
	//	for (size_t systemIndex = 0; systemIndex < NUM_SYSTEM_TYPES; ++systemIndex)
	//	{
	//		System* system = m_Systems[systemIndex].get();
	//		if (system == nullptr)
	//		{
	//			continue; // This particular system was not created, which is fine
	//		}
	//		system->UpdateEntitySet(entity, entitySignature);
	//	}
	//}

	void UpdateEntitySets()
	{
		// Notify each system that an entity's signature changed
		for (auto& system : m_Systems)
		{
			system.second->UpdateEntitySet();
		}
	}

	void UpdateAllSystems(float deltaSecs)
	{
		for (auto& system : m_Systems)
		{
			system.second->Update(deltaSecs);
		}
	}

	void RenderAllSystems()
	{
		constexpr RenderPass renderPasses[] = { RenderPass::MAIN, RenderPass::DEBUG };
		for (auto pass : renderPasses)
		{
			for (auto& system : m_Systems)
			{
				system.second->Render(pass);
			}
		}
	}

private:

	// Map from SystemType to a system pointer
	std::map<SystemType, std::shared_ptr<System>> m_Systems;
};