#include "SDLInputSystem.h"

#include "KBControlComponent.h"
#include "World.h"

#include "SDL.h"


void SDLInputSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBCONTROL);
	m_ParentWorld->SetSystemSignature<SDLInputSystem>(sysSignature);
}


void SDLInputSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	for (EntityID e : mEntities)
	{
		auto& kbControl = m_ParentWorld->GetComponent<KBControlComponent>(e);

		bool testButtonPressed = keystate[SDL_SCANCODE_SPACE];

		kbControl.m_Jump = testButtonPressed;
	}

}