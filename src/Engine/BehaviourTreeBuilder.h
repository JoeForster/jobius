#include "BehaviourTree.h"

class BehaviourTreeBuilder
{
public:
	BehaviourTreeBuilder()
	: m_Tree(new BehaviourTree)
	, m_CurrentBehaviour(nullptr)
	{
	}

	// TODO: Specialisation for leaf nodes that auto-ends?
	template<typename T, typename... Args>
	BehaviourTreeBuilder& AddNode(Args... args)
	{
		Behaviour* newNode = new T(m_CurrentBehaviour, m_Tree->m_State, args...);
		Behaviour* nodeParent = newNode->GetParent();
		assert(newNode->GetParent() == m_CurrentBehaviour);
		if (m_Tree->m_Root == nullptr)
		{
			assert(nodeParent == nullptr);
			m_Tree->m_Root = newNode;
		}
		else
		{
			assert(nodeParent != nullptr);
			nodeParent->AddChild(newNode);
		}

		m_Tree->m_Behaviours.push_back(newNode);

		m_CurrentBehaviour = newNode;


		return *this;
	}

	BehaviourTreeBuilder& EndNode()
	{
		if (m_CurrentBehaviour == m_Tree->m_Root)
		{
			// done - root should have no parent
			assert(m_CurrentBehaviour->GetParent() == nullptr);
			m_CurrentBehaviour = nullptr;
		}
		else
		{
			m_CurrentBehaviour = m_CurrentBehaviour->GetParent();
		}

		return *this;
	}

	BehaviourTree* EndTree()
	{
		// We should have gone back to the root via EndNode
		assert(m_CurrentBehaviour == nullptr);
		assert(!m_Tree->m_State.IsStructureLocked);
		m_Tree->m_State.IsStructureLocked = true;
		return m_Tree;
	}

private:
	BehaviourTree* m_Tree;
	Behaviour* m_CurrentBehaviour;
};