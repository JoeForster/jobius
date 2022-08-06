#pragma once

#include <memory>

#include "WorldBuilder.h"

class SDLRenderManager;
class World;

class BlockoLifeWorldBuilder : public WorldBuilder
{
public:
	std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) final;
};
