#pragma once

#include "BehaviourTree.h"
#include "ComponentManager.h"

class Behaviour;

struct BehaviourTreeComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BEHAVIOURTREE; }

	// TODO take unique ptr
	BehaviourTreeComponent(BehaviourTreeInstance* tree): m_Tree(tree) {}
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

	// Currently we have a an instance per entity, but it could be a shared template now that the state is out of it)
	BehaviourTreeInstance* m_Tree = nullptr;
	// State of the tree for this entity
	BehaviourTreeState m_State;
};
