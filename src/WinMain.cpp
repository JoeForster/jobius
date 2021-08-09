#include <iostream>
#include <assert.h>

#include <SDL.h>

#include "EntityManager.h"
#include "ComponentManager.h"
#include "Coordinator.h"
#include "RenderSystem.h"


using namespace std;

struct Vector3f
{
	float x = 0;
	float y = 0;
	float z = 0;
};

struct Transform
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_TRANSFORM; }

	Transform()
	: m_Pos{0, 0, 0}, m_Rot{0, 0, 0}, m_Scale{0, 0, 0} {}

	Transform(Vector3f inPos, Vector3f inRot, Vector3f inScale)
	: m_Pos(inPos), m_Rot(inRot), m_Scale(inScale) {}

	Vector3f m_Pos;
	Vector3f m_Rot;
	Vector3f m_Scale;
};

// TODO unit tests go somewhere
void TestCoordinator()
{
	Coordinator c;
	c.Init();

	c.RegisterComponent<Transform>();

	auto rs = c.RegisterSystem<RenderSystem>();

	EntityID e = c.CreateEntity();
	Transform t( {1, 2, 3}, {4, 5, 6}, {7, 8, 9} );
	c.AddComponent<Transform>(e, t);

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
