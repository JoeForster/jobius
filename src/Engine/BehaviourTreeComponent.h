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

	BehaviourTree* m_Tree = nullptr;
};
