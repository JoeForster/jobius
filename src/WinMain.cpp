#include <iostream>
#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "World.h"
#include "RenderSystem.h"
#include "SDLRenderManager.h"
#include "SDLInputSystem.h"
#include "PlayerControlSystem.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "RigidBodyComponent.h"
#include "KBInputComponent.h"
#include "PadInputComponent.h"


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

	// Init game systems, managers

	std::shared_ptr<SDLRenderManager> renderMan = SDLRenderManager::Create(window, renderer);
	ResourceID resID_asteroid = renderMan->LoadImage("assets/sprites/asteroid.png");
	assert(resID_asteroid != SDLRenderManager::ResourceID_Invalid);
	ResourceID resID_fighter = renderMan->LoadImage("assets/sprites/fighter_lr.png");
	assert(resID_fighter != SDLRenderManager::ResourceID_Invalid);

	std::shared_ptr<World> world = std::make_shared<World>();

	world->RegisterComponent<TransformComponent>();
	world->RegisterComponent<SpriteComponent>();
	world->RegisterComponent<RigidBodyComponent>();
	world->RegisterComponent<KBInputComponent>();
	world->RegisterComponent<PadInputComponent>();

	// Init systems
	world->RegisterSystem<SDLInputSystem>()->Init();

	world->RegisterSystem<PlayerControlSystem>()->Init();

	world->RegisterSystem<PhysicsSystem>()->Init();


	auto rs = world->RegisterSystem<RenderSystem>();
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;
	rs->Init(renderInit);

	// Create test world entities
	auto createSprite = [&](ResourceID resID, Vector3f pos, bool hasPhysics)
	{
		EntityID e = world->CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		world->AddComponent<TransformComponent>(e, t);
		world->AddComponent<SpriteComponent>(e, resID);
		if (hasPhysics)
		{
			world->AddComponent<RigidBodyComponent>(e);
		}
		return e;
	};

	createSprite(resID_asteroid, { 50, 0, 0 }, false);
	createSprite(resID_asteroid, { 150, 0, 0 }, true);
	auto playerEntity = createSprite(resID_fighter, { 250, 0, 0 }, true);
	world->AddComponent<KBInputComponent>(playerEntity);
	world->AddComponent<PadInputComponent>(playerEntity);

	static constexpr float s_TargetFrameTime = 1.0f/60.0f; 

	float lastFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
	SDL_Event event;
	while (SDL_PollEvent(&event) >= 0)
	{
		float curFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
		float deltaSecs = curFrameTimeSecs - lastFrameTimeSecs;
		float waitSecs = (deltaSecs > s_TargetFrameTime ? 0.0f : s_TargetFrameTime - deltaSecs);

		world->Update(deltaSecs);
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
