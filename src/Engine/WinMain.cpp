#include <iostream>
#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "EntityManager.h"
#include "World.h"

#include "ComponentManager.h"
#include "SDLRenderManager.h"

#include "SpriteRenderSystem.h"
#include "GridSpriteRenderSystem.h"
#include "SDLInputSystem.h"
#include "PlayerControlSystem.h"
#include "NPCControlSystem.h"
#include "NPCSensorSystem.h"
#include "PhysicsSystem.h"
#include "BoxCollisionSystem.h"
#include "PlaneCollisionSystem.h"

#include "TransformComponent.h"
#include "GridTransformComponent.h"
#include "SpriteComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "PlaneComponent.h"
#include "KBInputComponent.h"
#include "PadInputComponent.h"
#include "NPCBlackboardComponent.h"
#include "PlayerComponent.h"
#include "DebugTextComponent.h"
#include "GridWorldComponent.h"

// TODO better compile time game switching, separate project?
//#define JOBIUS_SHELL_GAME ChaosSpace

#if JOBIUS_SHELL_GAME == BlockoLife
#	include "BlockoLifeWorldBuilder.h"
#elif JOBIUS_SHELL_GAME == ChaosSpace
#	include "ChaosSpaceWorldBuilder.h"
#else
#	error "Unrecognised JOBIUS_SHELL_GAME"
#endif

int main(int argc, char* argv[])
{
	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		assert(0 && "Failed to initialize video!");
		exit(-1);
	}

	// Logging for development
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	SDL_Window* window = SDL_CreateWindow("Jobius", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window)
	{
		assert(0 && "Failed to create window!");
		exit(-1);
	}

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	
	if (TTF_Init() == -1)
	{
		assert(0 && "Failed to create ttf!");
		exit(-1);
	}
	
	// Load common managers and resources
	std::shared_ptr<SDLRenderManager> renderMan = SDLRenderManager::Create(window, renderer);
	ResourceID resID_font = renderMan->LoadFont("assets/fonts/FreeMono.ttf");
	assert(resID_font != ResourceID_Invalid);

	// Load the game world.
#if JOBIUS_SHELL_GAME == BlockoLife
	BlockoLifeWorldBuilder builder;	
#elif JOBIUS_SHELL_GAME == ChaosSpace
	ChaosSpaceWorldBuilder builder;	
#else
#	error "Unrecognised JOBIUS_SHELL_GAME"
#endif

	std::shared_ptr<World> world = builder.BuildWorld(renderMan);

	// SDL: Run the main game loop.
	static constexpr float s_TargetFrameTime = 1.0f/60.0f; 

	float lastFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
	SDL_Event event;
	while (SDL_PollEvent(&event) >= 0)
	{
		float curFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
		float deltaSecs = curFrameTimeSecs - lastFrameTimeSecs;
		float waitSecs = (deltaSecs > s_TargetFrameTime ? 0.0f : s_TargetFrameTime - deltaSecs);

		world->Update(deltaSecs);

		// TODO_RENDER_SYSTEM this should be part of a unified render system or schedule?
		renderMan->RenderClear();

		world->Render();

		SDL_RenderPresent(renderer);
		Uint32 delayMs = (Uint32)(waitSecs * 1000.0f);
		SDL_Delay(delayMs);

		lastFrameTimeSecs = curFrameTimeSecs;
	}

	// TODO cleanup game

	SDL_Quit();

	return 0;
}
