#include "InputSystem.h"

#include "RigidBodyComponent.h"
#include "KBControlComponent.h"
#include "World.h"

#include "SDL.h"


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


	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	for (EntityID e : mEntities)
	{
		auto& rigidBody = m_ParentWorld->GetComponent<RigidBodyComponent>(e);
		auto& kbControl = m_ParentWorld->GetComponent<KBControlComponent>(e);

		bool testButtonPressed = keystate[SDL_SCANCODE_SPACE];

		kbControl.m_Jump = testButtonPressed;
	
		// HACK TEST
		// TODO this might belong in a "control system" rather than input->physics direct dependency.
		rigidBody.m_Mass = kbControl.m_Jump ? 0.0f : 1.0f;
	}

}