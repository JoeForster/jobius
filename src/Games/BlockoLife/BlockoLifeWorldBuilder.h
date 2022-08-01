#pragma once

// FIXME
#include "..\..\Engine\WorldBuilder.h"
#include "memory"                       // for shared_ptr
class SDLRenderManager;
class World;
class BlockoLifeWorldBuilder : public WorldBuilder
{
public:
	std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) final;
};
