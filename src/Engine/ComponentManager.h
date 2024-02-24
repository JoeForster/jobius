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
	inline ComponentType GetComponentType() const
	{
		return T::GetComponentType();
	}

	template<typename T>
	inline size_t GetComponentIndex() const
	{
		return (size_t)T::GetComponentType();
	}
	
	template<typename T>
	inline size_t GetComponentCount() const
	{
		return GetComponentArray<T>().GetNumEntries();
	}

	template<typename T>
	void AddComponent(EntityID entity, T&& component)
	{
		// Add a component to the array for an entity
		GetComponentArray<T>().InsertData(entity, std::move(component));
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		// Remove a component from the array for an entity
		assert(false && "Untested route");
		GetComponentArray<T>().RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>().GetData(entity);
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

	// TODO would be even better to return a reference
	//template<typename T>
	//ComponentArray<T>* GetComponentArray()
	//{
	//	return static_cast<ComponentArray<T>*>(m_ComponentArrays[GetComponentIndex<T>()].get());
	//}
	template<typename T>
	ComponentArray<T>& GetComponentArray()
	{
		return *static_cast<ComponentArray<T>*>(m_ComponentArrays[GetComponentIndex<T>()].get());
	}
	template<typename T>
	const ComponentArray<T>& GetComponentArray() const
	{
		return *static_cast<const ComponentArray<T>*>(m_ComponentArrays[GetComponentIndex<T>()].get());
	}

};