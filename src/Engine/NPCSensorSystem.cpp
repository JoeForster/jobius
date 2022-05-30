#include "NPCSensorSystem.h"

#include <set>

#include "NPCBlackboardComponent.h"
#include "TransformComponent.h"
#include "PlayerComponent.h"
#include "World.h"



void NPCSensorSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);

	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_BLACKBOARD_NPC);
	m_ParentWorld->SetSystemSignature<NPCSensorSystem>(sysSignature);
}


void NPCSensorSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// Sensor detects players
	// TODO: Cache result (maybe the query or world itself should do that for us)
	EntitySignature querySignature;
	querySignature.set((size_t)ComponentType::CT_TRANSFORM);
	querySignature.set((size_t)ComponentType::CT_PLAYER);
	EntityQuery q (querySignature);
	std::set<EntityID> playerEntities;
	m_ParentWorld->ExecuteQuery(q, playerEntities);

	// TODO_AI_MULTIPLE_PLAYERS temp hack - bb should have all players and behaviour system should choose which.
	EntityID targetPlayer = INVALID_ENTITY_ID;
	const Vector3f* playerPos = nullptr;
	for (EntityID e : playerEntities)
	{
		auto& player = m_ParentWorld->GetComponent<PlayerComponent>(e);
		if (player.m_PlayerIndex == 0)
		{
			targetPlayer = e;
			assert(targetPlayer != INVALID_ENTITY_ID);
			const auto& playerTransform = m_ParentWorld->GetComponent<TransformComponent>(targetPlayer);
			playerPos = &playerTransform.m_Pos;
			break;
		}

	}
	if (targetPlayer == INVALID_ENTITY_ID)
	{
		return;
	}

	for (EntityID e : mEntities)
	{
		auto& npcBB = m_ParentWorld->GetComponent<NPCBlackboardComponent>(e);

		npcBB.PlayerPos.x = playerPos->x;
		npcBB.PlayerPos.y = playerPos->y;
	}
}
