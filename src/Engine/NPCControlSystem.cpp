#include "NPCControlSystem.h"

#include "NPCBlackboardComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "World.h"
#include "BehaviourTreeBuilder.h"
#include "Behaviours.h"


// TODO HACK STATICS single-NPC test for now

struct AIConfig
{
	float ArriveDistSq = 5.f * 5.f;
	float VisionRadiusSq = 200.f * 200.f;
	float AttackRadiusSq = 20.f * 20.f;
};

static AIConfig s_AIConfig;

struct Blackboard
{
	Vector2f NPCPos {};
	Vector2f NPCTargetPos {};
	bool NPCTargetPosSet = false;
	Vector2f PlayerPos {};
	Vector2f PlayerLastKnownPos {};
	bool PlayerLastKnownPosSet = false;
	bool NPCFired = false;
};

static Blackboard s_Blackboard;

class CheckPlayerVisible : public Condition
{
public:
	CheckPlayerVisible(Behaviour* parent, BehaviourTreeState& treeState)
	: Condition(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		if (s_Blackboard.NPCPos.DistSq(s_Blackboard.PlayerPos) < s_AIConfig.VisionRadiusSq)
		{
			m_Status = BehaviourStatus::SUCCESS;
			s_Blackboard.PlayerLastKnownPosSet = true;
			s_Blackboard.PlayerLastKnownPos = s_Blackboard.PlayerPos;
		}
		else
		{
			m_Status = BehaviourStatus::FAILURE;
		}
		return m_Status;
	}
};

class IsPlayerInRange : public Condition
{
public:
	IsPlayerInRange(Behaviour* parent, BehaviourTreeState& treeState)
	: Condition(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		if (s_Blackboard.NPCPos.DistSq(s_Blackboard.PlayerPos) < s_AIConfig.AttackRadiusSq)
		{
			m_Status = BehaviourStatus::SUCCESS;
		}
		else
		{
			m_Status = BehaviourStatus::FAILURE;
		}

		m_Status = BehaviourStatus::FAILURE;
		return m_Status;
	}
};

class FireAtPlayer : public Action
{
public:
	FireAtPlayer(Behaviour* parent, BehaviourTreeState& treeState)
	: Action(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		s_Blackboard.NPCFired = true;
		m_Status = BehaviourStatus::SUCCESS;
		return m_Status;
	}
};

class MoveToPlayersLKP : public Action
{
public:
	MoveToPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
	: Action(parent, treeState)
	{}

	BehaviourStatus Update() override
	{

		if (!s_Blackboard.NPCTargetPosSet)
		{
			if (s_Blackboard.PlayerLastKnownPosSet)
			{
				s_Blackboard.NPCTargetPos = s_Blackboard.PlayerLastKnownPos;
				s_Blackboard.NPCTargetPosSet = true;
			}
		}

		if (s_Blackboard.NPCTargetPosSet)
		{
			if (s_Blackboard.NPCPos.DistSq(s_Blackboard.NPCTargetPos) < s_AIConfig.ArriveDistSq)
			{
				s_Blackboard.NPCTargetPos = s_Blackboard.NPCPos;
				s_Blackboard.NPCTargetPosSet = false;
				m_Status = BehaviourStatus::SUCCESS;
			}
			else
			{
				m_Status = BehaviourStatus::RUNNING;
			}
		}
		else
		{
			m_Status = BehaviourStatus::FAILURE;
		}
		
		return m_Status;
	}
};

class LookAround : public Action
{
public:
	LookAround(Behaviour* parent, BehaviourTreeState& treeState)
	: Action(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		m_Status = BehaviourStatus::SUCCESS;
		return m_Status;
	}
};

class CheckHasPlayersLKP : public Condition
{
public:
	CheckHasPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
	: Condition(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		if (s_Blackboard.PlayerLastKnownPosSet)
		{
			m_Status = BehaviourStatus::SUCCESS;
		}
		else
		{
			m_Status = BehaviourStatus::FAILURE;
		}
		return m_Status;
	}
};

class MoveToRandomPosition : public Action
{
public:
	MoveToRandomPosition(Behaviour* parent, BehaviourTreeState& treeState)
	: Action(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		m_Status = BehaviourStatus::SUCCESS;
		return m_Status;
	}
};


void NPCControlSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_BLACKBOARD_NPC);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	m_ParentWorld->SetSystemSignature<NPCControlSystem>(sysSignature);
	m_ParentWorld->SetSystemDebugSignature<NPCControlSystem>(sysSignature);
}


void NPCControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : mEntities)
	{
		auto& npcBB = m_ParentWorld->GetComponent<NPCBlackboardComponent>(e);
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		auto& npcTransform = m_ParentWorld->GetComponent<TransformComponent>(e);

		// TODO_AI_BLACKBOARD validation if no player?
		s_Blackboard.PlayerPos.x = npcBB.PlayerPos.x;
		s_Blackboard.PlayerPos.y = npcBB.PlayerPos.y;
		s_Blackboard.NPCPos.x = npcTransform.m_Pos.x;
		s_Blackboard.NPCPos.y = npcTransform.m_Pos.y;

		// TODO tree per NPC
		// (manage separately in system for now - then  need to figure out how to ECS/componentise a BT)
		static BehaviourTree* bt = nullptr;
		
		if (bt == nullptr)
		{
			bt = BehaviourTreeBuilder()
				.AddNode<ActiveSelector>() // Root
					.AddNode<Sequence>() // Attack the player if seen
						.AddNode<CheckPlayerVisible>().EndNode()
						.AddNode<ActiveSelector>()
							.AddNode<Sequence>()
								.AddNode<IsPlayerInRange>().EndNode()
								.AddNode<Repeat>(3)
									.AddNode<FireAtPlayer>().EndNode()
								.EndNode()
							.EndNode()
						.EndNode()
					.EndNode() // End sequence: Attack player if seen
					.AddNode<Sequence>() // Search near last-known position
						.AddNode<CheckHasPlayersLKP>().EndNode()
						.AddNode<MoveToPlayersLKP>().EndNode()
						.AddNode<LookAround>().EndNode()
					.EndNode() // End sequence: search near last-known position
					.AddNode<Sequence>() // Random wander
						.AddNode<MoveToRandomPosition>().EndNode()
						.AddNode<LookAround>().EndNode()
					.EndNode() // End sequence: random wander
				.EndNode() // End root active selector
				.EndTree();

			bt->Start();
		}

		// Tick the static tree
		bt->Tick();
		// TODO_DEBUG_DRAW
		//bt->DebugToStream(std::cout) << std::endl;
		std::cout << "LastActiveNode: ";
		const Behaviour* an = bt->GetState().LastActiveNode;
		//if (an != nullptr)
		{
			an->DebugToStream(std::cout) << std::endl;
		}
		//else
		//{
		//	std::cout << "NULL" << std::endl;
		//}

		// TODO_AI_STEERING the steering/locomotion controller would be responsible for the below

		rigidBody.m_Vel.x = 0.f;
		rigidBody.m_Vel.y = 0.f;
		
		if (s_Blackboard.NPCTargetPosSet)
		{
			Vector2f controlVel = s_Blackboard.NPCTargetPos - s_Blackboard.NPCPos;
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
