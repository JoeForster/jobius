#include "NPCControlSystem.h"

#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "RigidBodyComponent.h"
#include "World.h"
#include "BehaviourTreeBuilder.h"
#include "Behaviours.h"


// STUBS - need linking up with sensor/behaviour.


struct Blackboard
{
	Vector3f NPCPosition {};
	Vector3f NPCTargetPosition {};
	Vector3f PlayerPos {};
	Vector3f PlayerLastKnownPos {};
	bool PlayerLastKnownPosSet = false;
	bool NPCFired = false;
};

static Blackboard s_Blackboard;

class IsPlayerVisible : public Condition
{
public:
	IsPlayerVisible(Behaviour* parent, const BehaviourTreeState& treeState)
	: Condition(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		// TODO: Vector dist
		if (false)//s_Blackboard.NPCPosition.DistanceTo(s_Blackboard.PlayerPos) < s_VisionRadius)
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

class IsPlayerInRange : public Condition
{
public:
	IsPlayerInRange(Behaviour* parent, const BehaviourTreeState& treeState)
	: Condition(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		// TODO: Vector dist
		if (false)//s_Blackboard.NPCPosition.DistanceTo(s_Blackboard.PlayerPos) < s_AttackRadius)
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
	FireAtPlayer(Behaviour* parent, const BehaviourTreeState& treeState)
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
	MoveToPlayersLKP(Behaviour* parent, const BehaviourTreeState& treeState)
	: Action(parent, treeState)
	{}

	BehaviourStatus Update() override
	{
		if (s_Blackboard.PlayerLastKnownPosSet)
		{
			m_Status = BehaviourStatus::FAILURE;
		}
		// TODO: Vector dist
		else if (false)//s_Blackboard.NPCPosition.distanceTo(
		{
			m_Status = BehaviourStatus::SUCCESS;
		}
		else
		{
			s_Blackboard.NPCTargetPosition = s_Blackboard.NPCPosition;
			m_Status = BehaviourStatus::RUNNING;
		}
		return m_Status;
	}
};

class LookAround : public Action
{
public:
	LookAround(Behaviour* parent, const BehaviourTreeState& treeState)
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
	CheckHasPlayersLKP(Behaviour* parent, const BehaviourTreeState& treeState)
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
	MoveToRandomPosition(Behaviour* parent, const BehaviourTreeState& treeState)
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
	sysSignature.set((size_t)ComponentType::CT_NPCBHV);
	sysSignature.set((size_t)ComponentType::CT_RIGIDBODY);
	m_ParentWorld->SetSystemSignature<NPCControlSystem>(sysSignature);
}


void NPCControlSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	for (EntityID e : mEntities)
	{
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);

		// TODO plug in behaviour tree actions/queries and test
		//Vector2f desiredVel;

		static BehaviourTree* bt = BehaviourTreeBuilder()
			.AddNode<ActiveSelector>() // Root
				.AddNode<Sequence>() // Attack the player if seen
					.AddNode<IsPlayerVisible>().EndNode()
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

		// HACK TEST movement just to prove this is plugged in correctly
		static float counter = 0;
		static float vel = 1.0f;

		counter += deltaSecs;
		if (counter >= 2.0f)
		{
			counter = 0.0f;
			vel *= -1.0f;
		}
		rigidBody.m_Vel.x = vel;

		//if (UP)) rigidBody.m_Vel.y -= 1.0f;
		//if (DOWN)) rigidBody.m_Vel.y += 1.0f;
		//if (LEFT)) rigidBody.m_Vel.x -= 1.0f;
		//if (RIGHT)) rigidBody.m_Vel.x += 1.0f;

		rigidBody.m_Mass = 0.0f;
		rigidBody.m_Kinematic = true;
	}
}
