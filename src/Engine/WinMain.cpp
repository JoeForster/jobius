#include <iostream>
#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>


#include "EntityManager.h"
#include "ComponentManager.h"
#include "World.h"
#include "SpriteRenderSystem.h"
#include "SDLRenderManager.h"
#include "SDLInputSystem.h"
#include "PlayerControlSystem.h"
#include "PhysicsSystem.h"
#include "BoxCollisionSystem.h"
#include "PlaneCollisionSystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "RigidBodyComponent.h"
#include "AABBComponent.h"
#include "PlaneComponent.h"
#include "KBInputComponent.h"
#include "PadInputComponent.h"


// TODO: Move unit tests into their own project/subfolder
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE( "1: All test cases reside in other .cpp files (empty)", "[multi-file:1]" ) {
}

int _FIXME_main(int argc, char* argv[])
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

	// Unit Tests here for now



	// Init game systems, managers

	std::shared_ptr<SDLRenderManager> renderMan = SDLRenderManager::Create(window, renderer);
	ResourceID resID_asteroid = renderMan->LoadTexture("assets/sprites/asteroid.png");
	assert(resID_asteroid != SDLRenderManager::ResourceID_Invalid);
	ResourceID resID_fighter = renderMan->LoadTexture("assets/sprites/fighter_lr.png");
	assert(resID_fighter != SDLRenderManager::ResourceID_Invalid);

	std::shared_ptr<World> world = std::make_shared<World>();

	world->RegisterComponent<TransformComponent>();
	world->RegisterComponent<SpriteComponent>();
	world->RegisterComponent<RigidBodyComponent>();
	world->RegisterComponent<AABBComponent>();
	world->RegisterComponent<PlaneComponent>();
	world->RegisterComponent<KBInputComponent>();
	world->RegisterComponent<PadInputComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<PlayerControlSystem>()->Init();
	world->RegisterSystem<PhysicsSystem>()->Init(renderInit);
	world->RegisterSystem<BoxCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<PlaneCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<SpriteRenderSystem>()->Init(renderInit);

	// Create test world entities
	auto createSprite = [](World& w, ResourceID resID, Vector3f pos)
	{
		EntityID e = w.CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createSpriteWithPhysics = [](World& w, ResourceID resID, Vector3f pos, Vector2f boxSize, Vector2f boxOffset)
	{
		EntityID e = w.CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		w.AddComponent<RigidBodyComponent>(e);
		w.AddComponent<AABBComponent>(e, { boxSize, boxOffset } );
		return e;
	};

	createSprite(*world, resID_asteroid, { 50, 0, 0 });
	createSpriteWithPhysics(*world, resID_asteroid, { 150, 0, 0 }, Vector2f{60.0f, 50.0f}, Vector2f{25.0f, 25.0f});

	auto player1Entity = createSpriteWithPhysics(*world, resID_fighter, { 500, 0, 0 }, Vector2f{200.0f, 100.0f}, Vector2f{50.0f, 25.0f});
	world->AddComponent<KBInputComponent>(player1Entity);
	world->AddComponent<PadInputComponent>(player1Entity, {0});

	auto player2Entity = createSpriteWithPhysics(*world, resID_fighter, { 700, 0, 0 }, Vector2f{200.0f, 100.0f}, Vector2f{50.0f, 25.0f});
	world->AddComponent<KBInputComponent>(player2Entity);
	world->AddComponent<PadInputComponent>(player2Entity, {1});

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
