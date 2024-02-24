#pragma once

#include <memory>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

// Simplistic query: given a signature, return entities with at least those components
// TODO: If the common case is "pre-query once and keep entity list, updating when entities are added",
// then does it make sense to hold an entity list in here and act like SystemManager::OnEntitySignatureChanged?
class EntityQuery
{
public:
	EntityQuery(EntitySignature signature): m_QuerySignature(signature) {}

	bool CheckEntity(EntitySignature entitySignature) const
	{
		return (m_QuerySignature & entitySignature) == m_QuerySignature;
	}

private:
	EntitySignature m_QuerySignature;
};


class World : public std::enable_shared_from_this<World>
{
public:

	// Entity methods
	EntityID CreateEntity()
	{
		m_SystemManager.SetEntitySetsDirty();
		return m_EntityManager.CreateEntity();
	}

	void DestroyEntity(EntityID entity)
	{
		m_EntityManager.DestroyEntity(entity);
		m_ComponentManager.OnEntityDestroyed(entity);
		m_SystemManager.OnEntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		m_ComponentManager.RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(EntityID entity, T&& component = {})
	{
		m_ComponentManager.AddComponent<T>(entity, std::move(component));

		auto signature = m_EntityManager.GetSignature(entity);
		signature.set(m_ComponentManager.GetComponentIndex<T>(), true);
		m_EntityManager.SetSignature(entity, signature);
		
		m_SystemManager.SetEntitySetsDirty();
	}

	template<typename T>
	void SetGlobalComponent(T&& component = {})
	{
		// A global component works just like a regular component of which there is one per world
		assert(m_ComponentManager.GetComponentCount<T>() == 0 && "SetGlobalComponent called on component that already exists");
		AddComponent<T>(0, std::move(component));
		
		m_SystemManager.SetEntitySetsDirty();
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		assert(false && "Untested route");
		m_ComponentManager.RemoveComponent<T>(entity);

		auto signature = m_EntityManager.GetSignature(entity);
		signature.set(m_ComponentManager.GetComponentType<T>(), false);
		m_EntityManager.SetSignature(entity, signature);
		
		m_SystemManager.SetEntitySetsDirty();
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		return m_ComponentManager.GetComponent<T>(entity);
	}

	template<typename T>
	T& GetGlobalComponent()
	{
		assert(m_ComponentManager.GetComponentCount<T>() > 0 && "GetGlobalComponent called for unset component");
		assert(m_ComponentManager.GetComponentCount<T>() == 1 && "GetGlobalComponent called for component with multiple entries");
		return GetComponent<T>(0);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return m_ComponentManager.GetComponentType<T>();
	}

	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return m_SystemManager.RegisterSystem<T>(shared_from_this());
	}

	template<typename T>
	void SetSystemSignatures(EntitySignature signature)
	{
		m_SystemManager.SetSignatures<T>(signature, signature);
	}

	template<typename T>
	void SetSystemSignatures(EntitySignature signature, EntitySignature debugSignature)
	{
		m_SystemManager.SetSignatures<T>(signature, debugSignature);
	}

	void ExecuteQuery(EntityQuery query, std::set<EntityID>& entitiesOut)
	{
		m_EntityManager.ExecuteQuery(query, entitiesOut);
	}

	void UpdateEntitySets()
	{
		// TODO_ENTITY_CREATION in order to create/update entities after construction we need to call this,
		// ideally at a fixed point per frame/tick automatically.
		m_SystemManager.UpdateEntitySets();
	}

	void Update(float deltaSecs)
	{
		m_SystemManager.UpdateAllSystems(deltaSecs);
	}

	void Render()
	{
		m_SystemManager.RenderAllSystems();
	}

private:
	ComponentManager m_ComponentManager;
	EntityManager m_EntityManager;
	SystemManager m_SystemManager;
};