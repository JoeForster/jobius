#pragma once

#include "EntityManager.h"

#include <assert.h>

// An interface is needed so that the ComponentManager
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void OnEntityDestroyed(EntityID entity) = 0;
};

static size_t INVALID_COMPONENT_INDEX = ~0u;

template<class T>
class ComponentArray : public IComponentArray
{
public:
	ComponentArray()
	{
		static_assert(T::GetComponentType() < ComponentType::CT_MAX);

		for (size_t& ix : m_EntityToIndexMap) ix = INVALID_COMPONENT_INDEX;
		for (EntityID& e : m_IndexToEntityMap) e = INVALID_ENTITY_ID;
	}

	//static ComponentType GetComponentType() const override final { return T::GetComponentType(); }

	void InsertData(EntityID entity, T component)
	{
		// TODO_VALIDATION
		//assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = m_NumEntries++;
		m_EntityToIndexMap[entity] = newIndex;
		m_IndexToEntityMap[newIndex] = entity;
		m_ComponentArray[newIndex] = component;
	}

	void RemoveData(EntityID entity)
	{
		assert(EntityManager::ValidEntityID(entity));
		// TODO_VALIDATION
		//assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");
		assert(m_NumEntries > 0);

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
		size_t indexOfLastElement = m_NumEntries - 1;
		m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

		// Update map to point to moved spot
		EntityID entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
		m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		m_EntityToIndexMap[entity] = INVALID_ENTITY_ID;
		m_IndexToEntityMap[indexOfLastElement] = INVALID_ENTITY_ID;

		--m_NumEntries;
	}

	T& GetData(EntityID entity)
	{
		// TODO_VALIDATION
		//assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Retrieving non-existent component.");

		// Return a reference to the entity's component
		return m_ComponentArray[m_EntityToIndexMap[entity]];
	}

	void OnEntityDestroyed(EntityID entity) override
	{
		// TODO_VALIDATION
		// Quick fix just to allow removing an entity that doesn't have ALL components.. This whole thing needs a look for partial component lists
		if (m_EntityToIndexMap[entity] != INVALID_COMPONENT_INDEX)
		{
			RemoveData(entity);
		}
	}

	size_t GetNumEntries() const { return m_NumEntries; }

private:
	T m_ComponentArray[MAX_ENTITIES];

	// Map from an entity ID to an array index.
	size_t m_EntityToIndexMap[MAX_ENTITIES];

	// Map from an array index to an entity ID.
	EntityID m_IndexToEntityMap[MAX_ENTITIES];

	size_t m_NumEntries = 0;
};