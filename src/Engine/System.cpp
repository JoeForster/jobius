#include "System.h"

#include "World.h"

void System::UpdateEntitySet()
{
	m_ParentWorld->ExecuteQuery({ m_Signature }, m_Entities);

	if (m_Signature == m_DebugSignature)
	{
		m_EntitiesDebug = m_Entities;
	}
	else
	{
		m_ParentWorld->ExecuteQuery({ m_DebugSignature }, m_EntitiesDebug);	
	}

	m_EntitiesDirty = false;
}
