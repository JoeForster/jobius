#pragma once

#include <cstdint>
#include <bitset>
#include <queue>
#include <set>

enum class ComponentType
{
	CT_TRANSFORM,
	CT_GRIDTRANSFORM,
	CT_SPRITE,
	CT_RIGIDBODY,
	CT_AABB,
	CT_PLANE,
	CT_KBINPUT,
	CT_PADINPUT,
	CT_BLACKBOARD_NPC,
	CT_PLAYER,
	CT_DEBUGTEXT,
	CT_GRIDWORLD,
	CT_CAMERA_2D,
	CT_BEHAVIOURTREE,
	CT_BEHAVIOURNODE,
	// TODO: Game-specific components below, requires a better system to keep them in game code
	CT_BL_SPECIES,
	CT_BL_HEALTH,
	CT_BL_DROPPER,
	CT_MAX
};

constexpr size_t NUM_COMPONENT_TYPES = (size_t)ComponentType::CT_MAX;

using EntityID = std::uint32_t;
using EntitySignature = std::bitset<NUM_COMPONENT_TYPES>;

// NOTE can't keep putting this up due to hard array size limit of 0x7fffffff (2,147,483,647) bytes
// for expanding grid worlds may need to reconsider ordering or 
constexpr size_t MAX_ENTITIES = 10000000;
constexpr EntityID INVALID_ENTITY_ID = MAX_ENTITIES;

class EntityQuery;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	EntityID CreateEntity();
	void DestroyEntity(EntityID entityID);

	void SetSignature(EntityID entity, EntitySignature signature);
	EntitySignature GetSignature(EntityID entity);

	void ExecuteQuery(EntityQuery query, std::set<EntityID>& entitiesOut);

	static bool ValidEntityID(EntityID entity)
	{
		return (entity != INVALID_ENTITY_ID && entity < MAX_ENTITIES);
	}

private:
	// All unused IDs (stack, take last first)
	EntityID m_AvailableEntities[MAX_ENTITIES];
	EntityID m_NumAvailableEntities;

	EntitySignature m_Signatures[MAX_ENTITIES];

};
