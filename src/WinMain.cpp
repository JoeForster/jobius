#include <iostream>
#include <assert.h>

#include <SDL.h>

#include "EntityManager.h"
#include "ComponentManager.h"

using namespace std;

struct DummyComponent
{
	int a;
};

int main(int argc, char* argv[])
{
	cout << "Ahoy" << endl;

	// ENTITY MANAGER TEST
	EntityManager em;
	EntityID testEntities[15];
	for (EntityID& e : testEntities)
	{
		e = em.CreateEntity();
	}
	
	// COMPONENT TEST

	ComponentArray<DummyComponent> testComponentArr;
	testComponentArr.InsertData(testEntities[0], {0});

	ComponentManager cm;
	cm.AddComponent(testEntities[0], DummyComponent{0});


	for (EntityID& e : testEntities)
	{
		if (e != INVALID_ENTITY_ID)
		{
			em.DestroyEntity(e);
		}
	}




	// EXIT TEST
	exit(0);


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

	//IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	//
	//if (TTF_Init() == -1)
	//{
	//	assert(0 && "Failed to create ttf!");
	//	exit(-1);
	//}
	//

	// TODO init game objects


	float lastFrameTimeMs = (float)SDL_GetTicks() * 0.001f;
	SDL_Event event;
	while (SDL_PollEvent(&event) >= 0)
	{
		float curFrameTimeMs = (float)SDL_GetTicks() * 0.001f;
		float elapsedMs = curFrameTimeMs - lastFrameTimeMs;

		// TODO update game objects

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// TODO draw game

		lastFrameTimeMs = curFrameTimeMs;

		SDL_RenderPresent(renderer);
		SDL_Delay(1);
	}

	// TODO cleanup game

	SDL_Quit();

	return 0;
}
