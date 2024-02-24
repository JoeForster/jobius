#include "NPCControlSystem.h"

#include "BehaviourTreeComponent.h"
#include "NPCBlackboardComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "World.h"
#include "Behaviours.h"

void NPCControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_BEHAVIOURTREE);
	sysSignature.set((size_t)ComponentType::CT_BLACKBOARD_NPC);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	m_ParentWorld->SetSystemSignatures<NPCControlSystem>(sysSignature);

}


void NPCControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : GetEntities())
	{
		auto& npcBTree = m_ParentWorld->GetComponent<BehaviourTreeComponent>(e);
		auto& npcBB = m_ParentWorld->GetComponent<NPCBlackboardComponent>(e);
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		auto& npcTransform = m_ParentWorld->GetComponent<TransformComponent>(e);

		// TODO_AI_BLACKBOARD validation if no player?
		npcBB.PlayerPos.x = npcBB.PlayerPos.x;
		npcBB.PlayerPos.y = npcBB.PlayerPos.y;
		npcBB.NPCPos.x = npcTransform.m_Pos.x;
		npcBB.NPCPos.y = npcTransform.m_Pos.y;

		// TODO tree per NPC
		// (manage separately in system for now - then  need to figure out how to ECS/componentise a BT)
		// TODO
		// Tick the static tree
		// TODO read BehaviourTreeComponent off entity
		
		if (npcBTree.m_Tree != nullptr)
		{
			npcBTree.m_Tree->Tick(npcBB);

			// TODO_DEBUG_DRAW
			//bt->DebugToStream(std::cout) << std::endl;
			//std::cout << "LastActiveNode: ";
			//const Behaviour* an = bt->GetState().LastActiveNode;
			//if (an != nullptr)
			//{
			//	an->DebugToStream(std::cout) << std::endl;
			//}
			//else
			//{
			//	std::cout << "NULL" << std::endl;
			//}
		}

		// TODO_AI_STEERING the steering/locomotion controller would be responsible for the below

		rigidBody.m_Vel.x = 0.f;
		rigidBody.m_Vel.y = 0.f;
		
		if (npcBB.NPCTargetPosSet)
		{
			Vector2f controlVel = npcBB.NPCTargetPos - npcBB.NPCPos;
			if (controlVel.MagnitudeSq() > 1.0f)
			{
				controlVel.SetMagnitude(0.5f);
				rigidBody.m_Vel = controlVel;
			}
		}

		rigidBody.m_Mass = 0.0f;
		rigidBody.m_Kinematic = true;
	}
}

void NPCControlSystem::Render_Debug()
{
	for (EntityID e : GetEntities())
	{
		// TODO
		//auto& t = m_ParentWorld->GetComponent<TransformComponent>(e);
		//auto& aabb = m_ParentWorld->GetComponent<AABBComponent>(e);
		//m_RenderMan->DrawRect((Rect2i)aabb.m_Box, WorldCoords::WorldToScreen(*m_ParentWorld, t.m_Pos));
	}
}
