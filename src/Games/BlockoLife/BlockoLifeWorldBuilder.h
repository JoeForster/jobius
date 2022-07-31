#pragma once

// FIXME
#include "..\..\Engine\WorldBuilder.h"

class BlockoLifeWorldBuilder : public WorldBuilder
{
public:
	std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) final;
};
