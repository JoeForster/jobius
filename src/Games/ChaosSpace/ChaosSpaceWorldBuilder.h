#pragma once

// FIXME
#include "..\..\Engine\WorldBuilder.h"

class ChaosSpaceWorldBuilder : public WorldBuilder
{
public:
	std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) final;
};
