#pragma once

#include <memory>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

struct Rect2D;

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
	void AddComponent(EntityID entity, T component = {})
	{
		m_ComponentManager.AddComponent<T>(entity, component);

		auto signature = m_EntityManager.GetSignature(entity);
		signature.set(m_ComponentManager.GetComponentIndex<T>(), true);
		m_EntityManager.SetSignature(entity, signature);

		m_SystemManager.OnEntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		m_ComponentManager.RemoveComponent<T>(entity);

		auto signature = m_EntityManager.GetSignature(entity);
		signature.set(m_ComponentManager.GetComponentType<T>(), false);
		m_EntityManager.SetSignature(entity, signature);

		m_SystemManager.OnEntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		return m_ComponentManager.GetComponent<T>(entity);
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
	void SetSystemSignature(EntitySignature signature)
	{
		m_SystemManager.SetSignature<T>(signature);
	}

	template<typename T>
	void SetSystemDebugSignature(EntitySignature signature)
	{
		m_SystemManager.SetDebugSignature<T>(signature);
	}

	void ExecuteQuery(EntityQuery query, std::set<EntityID>& entitiesOut)
	{
		m_EntityManager.ExecuteQuery(query, entitiesOut);
	}

	void Update(float deltaSecs)
	{
		m_SystemManager.UpdateAllSystems(deltaSecs);
	}

	void Render()
	{
		m_SystemManager.RenderAllSystems();
	}

	// World Settings
	const Rect2D& GetBounds() const;

private:
	ComponentManager m_ComponentManager;
	EntityManager m_EntityManager;
	SystemManager m_SystemManager;
};