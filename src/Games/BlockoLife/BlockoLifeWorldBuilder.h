#pragma once

#include <memory>

#include "WorldBuilder.h"

#include "EntityBuilder.h"

class SDLRenderManager;
class World;

class BlockoLifeWorldBuilder : public WorldBuilder
{
public:
	std::shared_ptr<World> BuildWorld(std::shared_ptr<SDLRenderManager> renderMan) final;

	// TODO make array or map (requires clunky shared_ptr casting and maybe enable_shared_from_list as we do components,
	// maybe do when making this not game-specific)
	static EntityBuilder<SpeciesIdentity<Species::PLANT>> plantBuilder;
	static EntityBuilder<SpeciesIdentity<Species::HERBIVORE>> herbivoreBuilder;
	static EntityBuilder<SpeciesIdentity<Species::CARNIVORE>> carnivoreBuilder;
};
