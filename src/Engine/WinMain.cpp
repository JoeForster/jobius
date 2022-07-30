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

	// Unit Tests here for now



	// Init game systems, managers
	// TODO split up game world creation for separate demos

	std::shared_ptr<SDLRenderManager> renderMan = SDLRenderManager::Create(window, renderer);
	ResourceID resID_asteroid = renderMan->LoadTexture("assets/sprites/asteroid.png");
	assert(resID_asteroid != ResourceID_Invalid);
	ResourceID resID_fighter = renderMan->LoadTexture("assets/sprites/fighter_lr.png");
	assert(resID_fighter != ResourceID_Invalid);
	ResourceID resID_ufo = renderMan->LoadTexture("assets/sprites/ufo.png");
	assert(resID_ufo != ResourceID_Invalid);

	ResourceID resID_plant = renderMan->LoadTexture("assets/sprites/plant_32.png");
	assert(resID_plant != ResourceID_Invalid);

	ResourceID resID_font = renderMan->LoadFont("assets/fonts/FreeMono.ttf");
	assert(resID_font != ResourceID_Invalid);


	std::shared_ptr<World> world = std::make_shared<World>();

	// TODO Currently we get a crash if we add a component we forgot to register. Can we check at compile time?
	world->RegisterComponent<TransformComponent>();
	world->RegisterComponent<GridTransformComponent>();
	world->RegisterComponent<SpriteComponent>();
	world->RegisterComponent<RigidBodyComponent>();
	world->RegisterComponent<AABBComponent>();
	world->RegisterComponent<PlaneComponent>();
	world->RegisterComponent<KBInputComponent>();
	world->RegisterComponent<PadInputComponent>();
	world->RegisterComponent<NPCBlackboardComponent>();
	world->RegisterComponent<PlayerComponent>();
	world->RegisterComponent<DebugTextComponent>();
	world->RegisterComponent<GridWorldComponent>();

	// Initialiser for systems that render
	RenderSystemInitialiser renderInit;
	renderInit.m_RenderMan = renderMan;

	// Init systems
	world->RegisterSystem<SDLInputSystem>()->Init();
	world->RegisterSystem<PlayerControlSystem>()->Init(renderInit);
	world->RegisterSystem<NPCControlSystem>()->Init();
	world->RegisterSystem<NPCSensorSystem>()->Init();
	world->RegisterSystem<PhysicsSystem>()->Init(renderInit);
	world->RegisterSystem<BoxCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<PlaneCollisionSystem>()->Init(renderInit);
	world->RegisterSystem<SpriteRenderSystem>()->Init(renderInit);
	world->RegisterSystem<GridSpriteRenderSystem>()->Init(renderInit);

	// Create GLOBAL components

	world->SetGlobalComponent<GridWorldComponent>( { Rect2D{ Vector2f{0, 0}, Vector2f{1000, 700} }, 32.0f } );

	// Create test world entities
	auto createSprite = [](World& w, ResourceID resID, Vector3f pos)
	{
		EntityID e = w.CreateEntity();
		const TransformComponent t(pos, { 0, 0, 0 }, { 1, 1, 1 });
		w.AddComponent<TransformComponent>(e, t);
		w.AddComponent<SpriteComponent>(e, resID);
		return e;
	};
	auto createGridSprite = [](World& w, ResourceID resID, Vector2i pos)
	{
		EntityID e = w.CreateEntity();
		const GridTransformComponent t(pos);
		w.AddComponent<GridTransformComponent>(e, t);
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
		w.AddComponent<DebugTextComponent>(e);
		return e;
	};
	int nextPlayerIndex = 0;
	auto createPlayer = [&](Vector3f pos)
	{
		auto playerEntity = createSpriteWithPhysics(*world, resID_fighter, pos, {120, 54}, {60, 27});
		world->AddComponent<KBInputComponent>(playerEntity);
		world->AddComponent<PadInputComponent>(playerEntity, {nextPlayerIndex});
		world->AddComponent<PlayerComponent>(playerEntity, {nextPlayerIndex});
		++nextPlayerIndex;
	};

	createSprite(*world, resID_asteroid, { 50, 0, 0 });
	createSpriteWithPhysics(*world, resID_asteroid, { 150, 0, 0 }, { 70, 60 }, { 35, 30 });

	createPlayer({ 500, 0, 0 });
	createPlayer({ 700, 0, 0 });

	auto ufoEntity = createSpriteWithPhysics(*world, resID_ufo, { 200, 200, 200 }, { 158, 48 }, { 79, 24 });
	world->AddComponent<NPCBlackboardComponent>(ufoEntity);


	// GRID TEST
	
	createGridSprite(*world, resID_plant, { 5, 5 });


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
