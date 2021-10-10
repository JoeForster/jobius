#include "SDLInputSystem.h"

#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "World.h"

#include "SDL.h"


void SDLInputSystem::Init(const SystemInitialiser& initialiser)
{
	System::Init(initialiser);
	
	EntitySignature sysSignature;
	sysSignature.set((size_t)ComponentType::CT_KBINPUT);
	sysSignature.set((size_t)ComponentType::CT_PADINPUT);
	m_ParentWorld->SetSystemSignature<SDLInputSystem>(sysSignature);
}


void SDLInputSystem::Update(float deltaSecs)
{
	System::Update(deltaSecs);

	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	// TODO need to init/connect pad.
	//const Uint8* keystate = SDL_GameControllerGetButton(NULL);

	for (EntityID e : mEntities)
	{
		auto& kbControl = m_ParentWorld->GetComponent<KBInputComponent>(e);

		kbControl.m_State.set((size_t)KB_KEY::KEY_UP, (bool)keystate[SDL_SCANCODE_UP]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_DOWN, (bool)keystate[SDL_SCANCODE_DOWN]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_LEFT, (bool)keystate[SDL_SCANCODE_LEFT]);
		kbControl.m_State.set((size_t)KB_KEY::KEY_RIGHT, (bool)keystate[SDL_SCANCODE_RIGHT]);

		auto& padControl = m_ParentWorld->GetComponent<PadInputComponent>(e);


	}
}