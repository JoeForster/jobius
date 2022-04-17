#include "BehaviourTree.h"

// TODO: Requires us to upgrade standard to C++20 - which needs some fixes to our codebase..
//template <typename T>
//concept BehaviourType = std::is_base_of<Behaviour, T>::value;

class BehaviourTreeBuilder
{
public:
	BehaviourTreeBuilder()
	: m_Tree(new BehaviourTree)
	, m_CurrentBehaviour(nullptr)
	{
	}

	// TODO: Template this (requires figuring out how to pass in construction params)
	// TODO: Can then validate at compile-time that adding children to non-leaf nodes only?
	template<typename T>
	BehaviourTreeBuilder& AddNode()
	{
		Behaviour* node = new T(m_CurrentBehaviour);
		return AddNode(node);
	}

	BehaviourTreeBuilder& AddNode_Mock(MockActionRule rule)
	{
		Behaviour* node = new MockAction(m_CurrentBehaviour, rule);
		return AddNode(node);
	}

	// TODO: Auto-end leaf nodes.
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
		return m_Tree;
	}

private:
	BehaviourTree* m_Tree;
	Behaviour* m_CurrentBehaviour;

	// TODO would be nice, but how would we do this with variable constructor params?
	//template<class T>
	//BehaviourTreeBuilder& AddNode()
	// Add a newly-constructed node (takes ownership of memory)
	BehaviourTreeBuilder& AddNode(Behaviour* newNode)
	{
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
};