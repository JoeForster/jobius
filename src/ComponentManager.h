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
		//const char* typeName = typeid(T).name();

		//assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");
		ComponentType ct = T::GetComponentType();
		assert(ct < ComponentType::CT_MAX);
		size_t index = (size_t)ct;
		assert(index < NUM_COMPONENT_TYPES);
		assert(m_ComponentArrays[index] == nullptr && "Registering component type more than once.");

		// Add this component type to the component type map
		//mComponentTypes.insert({ typeName, mNextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		//mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>(ct) });

		// Increment the value so that the next component registered will be different
		//++mNextComponentType;

		m_ComponentArrays[index] = std::make_shared<ComponentArray<T>>();
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		//const char* typeName = typeid(T).name();
		//
		//assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
		//
		//// Return this component's type - used for creating signatures
		//return mComponentTypes[typeName];

		return T::GetComponentType();
	}

	template<typename T>
	size_t GetComponentIndex()
	{
		return (size_t)T::GetComponentType();
	}

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		// Add a component to the array for an entity
		GetComponentArray<T>().InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		// Remove a component from the array for an entity
		GetComponentArray<T>().RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentArray<T>().GetData(entity);
	}

	void EntityDestroyed(EntityID entity)
	{
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (std::shared_ptr<IComponentArray> componentArr : m_ComponentArrays)
		{
			componentArr->EntityDestroyed(entity);
		}
	}

private:
	// TODO get rid of the unnecessaries here
	// TODO do we really need a shared pointer for the arrays, can just be unique and return refs?

	// Map from type string pointer to a component type
	//std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array
	//std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
	std::shared_ptr<IComponentArray> m_ComponentArrays[NUM_COMPONENT_TYPES];

	// The component type to be assigned to the next registered component - starting at 0
	//ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
/*
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
	}
*/
	//template<typename T>
	//ComponentArray<T>* GetComponentArray()
	//{
	//	// TODO need to validate type or do away with need to pass type as well as T here (use reflection or make a map?)
	//	return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[T::GetComponentType()]);
	//}

	template<typename T>
	ComponentArray<T>& GetComponentArray()
	{
		// TODO need to validate type or do away with need to pass type as well as T here (use reflection or make a map?)
		auto p = m_ComponentArrays[GetComponentIndex<T>()];
		auto pc = std::static_pointer_cast<ComponentArray<T>>(p);
		return *pc;
	}

};