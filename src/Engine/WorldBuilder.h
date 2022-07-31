#pragma once

#include <memory>

class World;
class SDLRenderManager;

class WorldBuilder
{
public:
	// FIXME remove (SDL) render dependency
	virtual std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) = 0;
};