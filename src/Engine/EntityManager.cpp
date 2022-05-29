#include "EntityManager.h"

#include <assert.h>

#include "World.h"

EntityManager::EntityManager()
: m_NumAvailableEntities(0)
{
	for (EntityID entityID = 0; entityID < MAX_ENTITIES; ++entityID)
	{
		m_AvailableEntities[m_NumAvailableEntities++] = entityID;
	}
}

EntityManager::~EntityManager()
{
	
}

EntityID EntityManager::CreateEntity()
{
	if (m_NumAvailableEntities == 0)
	{
		return INVALID_ENTITY_ID;
	}

	return m_AvailableEntities[--m_NumAvailableEntities];
}


void EntityManager::DestroyEntity(EntityID entityID)
{
	assert(entityID < MAX_ENTITIES);
	assert(m_NumAvailableEntities < MAX_ENTITIES);

	// Invalidate the destroyed entity's signature
	m_Signatures[entityID].reset();
	m_AvailableEntities[m_NumAvailableEntities++] = entityID;
}

void EntityManager::SetSignature(EntityID entity, EntitySignature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	m_Signatures[entity] = signature;
}

EntitySignature EntityManager::GetSignature(EntityID entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	return m_Signatures[entity];
}

void EntityManager::ExecuteQuery(EntityQuery query, std::set<EntityID>& entitiesOut)
{
	for (EntityID entity = 0; entity < m_TailID; ++entity)
	{
		// Entity signature matches system signature - insert into set
		if (query.CheckEntity(entity))
		{
			entitiesOut.insert(entity);
		}
	}
}