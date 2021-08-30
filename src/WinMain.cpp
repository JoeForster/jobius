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
#include "Transform.h"


using namespace std;

// TODO unit tests go somewhere
void TestWorld()
{
	World w;
	w.Init();

	w.RegisterComponent<Transform>();


	auto rs = w.RegisterSystem<RenderSystem>();

	EntitySignature renderSignature;
	renderSignature &= (size_t)ComponentType::CT_TRANSFORM;
	w.SetSystemSignature<RenderSystem>(renderSignature);

	EntityID e = w.CreateEntity();
	Transform t( {1, 2, 3}, {4, 5, 6}, {7, 8, 9} );
	w.AddComponent<Transform>(e, t);

	rs->mEntities.insert(e);

}

int main(int argc, char* argv[])
{
	cout << "Ahoy" << endl;

	//TestCoordinator();
	//TestEntities();
	//return 0;


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

	// TODO init game systems, managers

	SDLRenderManager* renderMan = SDLRenderManager::Create(window, renderer);
	ResourceID spriteID = renderMan->LoadImage("TestSprite.png");

	shared_ptr<World> world = std::make_shared<World>();
	world->Init();

	world->RegisterComponent<Transform>();

	auto rs = world->RegisterSystem<RenderSystem>();

	EntitySignature renderSignature;
	renderSignature &= (size_t)ComponentType::CT_TRANSFORM;
	world->SetSystemSignature<RenderSystem>(renderSignature);

	EntityID e = world->CreateEntity();
	Transform t({ 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 });
	world->AddComponent<Transform>(e, t);

	rs->mEntities.insert(e);

	// TODO init game objects


	float lastFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
	SDL_Event event;
	while (SDL_PollEvent(&event) >= 0)
	{
		float curFrameTimeSecs = (float)SDL_GetTicks() * 0.001f;
		float elapsedSecs = curFrameTimeSecs - lastFrameTimeSecs;

		// START update game

		//rs->
		rs->Update(curFrameTimeSecs);
		

		// END update game

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// START draw game

		static int x = 0;
		static int y = 0;

		x = (x + 1) % 100;
		y = (y + 1) % 100;

		//renderMan->Draw(spriteID, x, y);

		// TODO update in the system
		// TODO render in the system
		Transform& t = world->GetComponent<Transform>(e);
		t.m_Pos.x = (float)(((int)t.m_Pos.x + 1) % 100);
		t.m_Pos.y = (float)(((int)t.m_Pos.y + 1) % 100);
		renderMan->Draw(spriteID, (int)t.m_Pos.x, (int)t.m_Pos.y);

		// END draw game

		lastFrameTimeSecs = curFrameTimeSecs;

		SDL_RenderPresent(renderer);
		SDL_Delay(1);
	}

	// TODO cleanup game

	SDL_Quit();

	return 0;
}
