#pragma once

#include "BehaviourTree.h"
#include "ComponentManager.h"

class Behaviour;

struct BehaviourTreeComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BEHAVIOURTREE; }

	// TODO take unique ptr
	BehaviourTreeComponent(BehaviourTree* tree): m_Tree(tree) {}
	BehaviourTreeComponent() {}
	~BehaviourTreeComponent() { delete m_Tree; }

	BehaviourTreeComponent(const BehaviourTreeComponent& toCopy) = delete;
	BehaviourTreeComponent& operator=(const BehaviourTreeComponent&& toCopy) = delete;
	BehaviourTreeComponent(BehaviourTreeComponent&& toMove) noexcept
	{
		m_Tree = toMove.m_Tree;
		toMove.m_Tree = nullptr;
	};
	BehaviourTreeComponent& operator=(BehaviourTreeComponent&& toMove) noexcept
	{
		delete m_Tree;
		m_Tree = toMove.m_Tree;
		toMove.m_Tree = nullptr;
		return *this;
	}


	BehaviourTree* m_Tree = nullptr;
};
