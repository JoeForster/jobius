#include "TestBehaviourNodes.h"

#include "NPCBlackboardComponent.h"

CheckPlayerVisible::CheckPlayerVisible(Behaviour* parent, BehaviourTreeState& treeState)
: Condition(parent, treeState)
{
}

BehaviourStatus CheckPlayerVisible::Update(NPCBlackboardComponent& blackboard)
{
	if (blackboard.NPCPos.DistSq(blackboard.PlayerPos) < s_AIConfig.VisionRadiusSq)
	{
		m_Status = BehaviourStatus::SUCCESS;
		blackboard.PlayerLastKnownPosSet = true;
		blackboard.PlayerLastKnownPos = blackboard.PlayerPos;
	}
	else
	{
		m_Status = BehaviourStatus::FAILURE;
	}
	return m_Status;
}


IsPlayerInRange::IsPlayerInRange(Behaviour* parent, BehaviourTreeState& treeState)
: Condition(parent, treeState)
{
}

BehaviourStatus IsPlayerInRange::Update(NPCBlackboardComponent& blackboard)
{
	if (blackboard.NPCPos.DistSq(blackboard.PlayerPos) < s_AIConfig.AttackRadiusSq)
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


FireAtPlayer::FireAtPlayer(Behaviour* parent, BehaviourTreeState& treeState)
: Action(parent, treeState)
{
}

BehaviourStatus FireAtPlayer::Update(NPCBlackboardComponent& blackboard)
{
	blackboard.NPCFired = true;
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}


MoveToPlayersLKP::MoveToPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
: Action(parent, treeState)
{}

BehaviourStatus MoveToPlayersLKP::Update(NPCBlackboardComponent& blackboard)
{
	if (!blackboard.NPCTargetPosSet)
	{
		if (blackboard.PlayerLastKnownPosSet)
		{
			blackboard.NPCTargetPos = blackboard.PlayerLastKnownPos;
			blackboard.NPCTargetPosSet = true;
		}
	}

	if (blackboard.NPCTargetPosSet)
	{
		if (blackboard.NPCPos.DistSq(blackboard.NPCTargetPos) < s_AIConfig.ArriveDistSq)
		{
			blackboard.NPCTargetPos = blackboard.NPCPos;
			blackboard.NPCTargetPosSet = false;
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


LookAround::LookAround(Behaviour* parent, BehaviourTreeState& treeState)
: Action(parent, treeState)
{
}

BehaviourStatus LookAround::Update(NPCBlackboardComponent& blackboard)
{
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}


CheckHasPlayersLKP::CheckHasPlayersLKP(Behaviour* parent, BehaviourTreeState& treeState)
: Condition(parent, treeState)
{
}

BehaviourStatus CheckHasPlayersLKP::Update(NPCBlackboardComponent& blackboard)
{
	if (blackboard.PlayerLastKnownPosSet)
	{
		m_Status = BehaviourStatus::SUCCESS;
	}
	else
	{
		m_Status = BehaviourStatus::FAILURE;
	}
	return m_Status;
}


MoveToRandomPosition::MoveToRandomPosition(Behaviour* parent, BehaviourTreeState& treeState)
: Action(parent, treeState)
{}

BehaviourStatus MoveToRandomPosition::Update(NPCBlackboardComponent& blackboard)
{
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}