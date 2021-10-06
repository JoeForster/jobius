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
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "RigidBodyComponent.h"
#include "KBControlComponent.h"


int main(int argc, char* argv[])
{
	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
	world->RegisterComponent<KBControlComponent>();

	// InputSYstem init
	world->RegisterSystem<PhysicsSystem>()->Init();

	// PhysicsSystem init
	world->RegisterSystem<PhysicsSystem>()->Init();

	// RenderSystem Init
	auto rs = world->RegisterSystem<RenderSystem>();
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;
	rs->Init(renderInit);

	// Create test world entities
	auto createSprite = [&](ResourceID resID, Vector3f pos, bool hasPhysics)
	{
		EntityID e = world->CreateEntity();
		TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		world->AddComponent<TransformComponent>(e, t);
		world->AddComponent<SpriteComponent>(e, resID);
		if (hasPhysics)
		{
			world->AddComponent<RigidBodyComponent>(e, {});
		}
	};

	createSprite(resID_asteroid, { 50, 0, 0 }, false);
	createSprite(resID_asteroid, { 150, 0, 0 }, true);
	createSprite(resID_fighter, { 250, 0, 0 }, true);

	float lastFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
	SDL_Event event;
	while (SDL_PollEvent(&event) >= 0)
	{
		float curFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
		float deltaSecs = curFrameTimeSecs - lastFrameTimeSecs;

		world->Update(deltaSecs);

		world->Render();

		lastFrameTimeSecs = curFrameTimeSecs;

		SDL_RenderPresent(renderer);
		SDL_Delay(1);
	}

	// TODO cleanup game

	SDL_Quit();

	return 0;
}
