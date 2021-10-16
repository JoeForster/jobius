#pragma once

#include "System.h"

// Awkward SDL forward decl due to typedef..
struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;

class SDLInputSystem : public System
{
public:
	static constexpr SystemType GetSystemType() { return SystemType::ST_INPUT; }

	SDLInputSystem(std::shared_ptr<World> parentWorld)
		: System(parentWorld)
	{
	}

	void Init(const SystemInitialiser& = s_EmptyInitialiser) override;

	void Update(float deltaSecs) final override;

private:
	std::vector<SDL_GameController*> m_Controllers;
};
