#pragma once

#include <memory>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class World : public std::enable_shared_from_this<World>
{
public:
	void Init()
	{
		// Create pointers to each manager
		m_ComponentManager = std::make_unique<ComponentManager>();
		m_EntityManager = std::make_unique<EntityManager>();
		m_SystemManager = std::make_unique<SystemManager>();
	}


	// Entity methods
	EntityID CreateEntity()
	{
		return m_EntityManager->CreateEntity();
	}

	void DestroyEntity(EntityID entity)
	{
		m_EntityManager->DestroyEntity(entity);

		m_ComponentManager->EntityDestroyed(entity);

		m_SystemManager->EntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		m_ComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		m_ComponentManager->AddComponent<T>(entity, component);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentIndex<T>(), true);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		m_ComponentManager->RemoveComponent<T>(entity);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), false);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		return m_ComponentManager->GetComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return m_ComponentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return m_SystemManager->RegisterSystem<T>(shared_from_this());
	}

	template<typename T>
	void SetSystemSignature(EntitySignature signature)
	{
		m_SystemManager->SetSignature<T>(signature);
	}

private:
	// TODO remove unnecessary use of pointers
	std::unique_ptr<ComponentManager> m_ComponentManager;
	std::unique_ptr<EntityManager> m_EntityManager;
	std::unique_ptr<SystemManager> m_SystemManager;
};