#pragma once

#include "EntityManager.h"
#include "ComponentArray.h"

//#include <unordered_map>
#include <memory>

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent()
	{
		ComponentType ct = T::GetComponentType();
		assert(ct < ComponentType::CT_MAX);
		size_t index = (size_t)ct;
		assert(index < NUM_COMPONENT_TYPES);
		assert(m_ComponentArrays[index] == nullptr && "Registering component type more than once.");

		m_ComponentArrays[index] = std::make_shared<ComponentArray<T>>();
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return T::GetComponentType();
	}

	template<typename T>
	size_t GetComponentIndex()
	{
		return (size_t)T::GetComponentType();
	}
	
	template<typename T>
	size_t GetComponentCount()
	{
		return GetComponentArray<T>()->GetNumEntries();
	}

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		// Add a component to the array for an entity
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		// Remove a component from the array for an entity
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>()->GetData(entity);
	}

	void OnEntityDestroyed(EntityID entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (std::shared_ptr<IComponentArray> componentArr : m_ComponentArrays)
		{
			componentArr->OnEntityDestroyed(entity);
		}
	}

private:

	std::shared_ptr<IComponentArray> m_ComponentArrays[NUM_COMPONENT_TYPES];

	// TODO: This is a bit clunky. Find a way to have it return references, or just do away with the shared_ptr?
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		std::shared_ptr<IComponentArray> p = m_ComponentArrays[GetComponentIndex<T>()];
		std::shared_ptr<ComponentArray<T>> pc = std::static_pointer_cast<ComponentArray<T>>(p);
		return pc;
	}

};