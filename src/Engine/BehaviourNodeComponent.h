#pragma once

#include "BehaviourStatus.h"
#include "ComponentManager.h"

class Behaviour;

struct BehaviourNodeComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BEHAVIOURNODE; }

	// TODO take unique ptr
	BehaviourNodeComponent(Behaviour* node): m_Node(node) {}
	BehaviourNodeComponent() {}
	~BehaviourNodeComponent() { delete m_Node; }

	Behaviour* m_Node = nullptr;
	int m_NumChildren = 0;
	BehaviourStatus m_Status = BehaviourStatus::INVALID;
};
