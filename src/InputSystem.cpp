#include "InputSystem.h"

#include "TransformComponent.h"
#include "KBControlComponent.h"
#include "World.h"


void InputSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_TRANSFORM);
	sysSignature.set((size_t)ComponentType::CT_KBCONTROL);
	m_ParentWorld->SetSystemSignature<InputSystem>(sysSignature);
}


void InputSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	// TODO
/*
	for (EntityID e : mEntities)
	{
		//TransformComponent& t = m_ParentWorld->GetComponent<K>(e);
	}
*/
}