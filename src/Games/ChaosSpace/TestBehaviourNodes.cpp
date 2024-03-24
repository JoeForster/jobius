#include "TestBehaviourNodes.h"

#include "NPCBlackboardComponent.h"

CheckPlayerVisible::CheckPlayerVisible(Behaviour* parent, BehaviourTreeData& treeData)
: Condition(parent, treeData)
{
}

BehaviourStatus CheckPlayerVisible::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
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


IsPlayerInRange::IsPlayerInRange(Behaviour* parent, BehaviourTreeData& treeData)
: Condition(parent, treeData)
{
}

BehaviourStatus IsPlayerInRange::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
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


FireAtPlayer::FireAtPlayer(Behaviour* parent, BehaviourTreeData& treeData)
: Action(parent, treeData)
{
}

BehaviourStatus FireAtPlayer::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
{
	blackboard.NPCFired = true;
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}


MoveToPlayersLKP::MoveToPlayersLKP(Behaviour* parent, BehaviourTreeData& treeData)
: Action(parent, treeData)
{}

BehaviourStatus MoveToPlayersLKP::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
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


LookAround::LookAround(Behaviour* parent, BehaviourTreeData& treeData)
: Action(parent, treeData)
{
}

BehaviourStatus LookAround::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
{
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}


CheckHasPlayersLKP::CheckHasPlayersLKP(Behaviour* parent, BehaviourTreeData& treeData)
: Condition(parent, treeData)
{
}

BehaviourStatus CheckHasPlayersLKP::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
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


MoveToRandomPosition::MoveToRandomPosition(Behaviour* parent, BehaviourTreeData& treeData)
: Action(parent, treeData)
{}

BehaviourStatus MoveToRandomPosition::Update(BehaviourTreeState& treeState, NPCBlackboardComponent& blackboard)
{
	m_Status = BehaviourStatus::SUCCESS;
	return m_Status;
}