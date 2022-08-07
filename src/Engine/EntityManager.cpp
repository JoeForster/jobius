#include "EntityManager.h"

#include <assert.h>

#include "World.h"

EntityManager::EntityManager()
: m_NumAvailableEntities(0)
{
	for (EntityID entityID = MAX_ENTITIES-1; entityID != ~0u; --entityID)
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
	// I hate this, and I'm the one who wrote it..!
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
	// TODO fix to only loop over valid entities once we've made this ID array stuff more sane
	for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		// Entity signature matches system signature - insert into set
		EntitySignature signature = m_Signatures[entity];
		if (entity != INVALID_ENTITY_ID && query.CheckEntity(signature))
		{
			entitiesOut.insert(entity);
		}
	}
}