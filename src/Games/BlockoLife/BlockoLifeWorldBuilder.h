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

	static EntityID BuildEntity(World& world, Species species, const Vector2i& pos)
	{
		switch (species)
		{
		case Species::PLANT:
			return m_PlantBuilder.Build(world, pos);
		case Species::HERBIVORE:
			return m_HerbivoreBuilder.Build(world, pos);
		case Species::CARNIVORE:
			return m_CarnivoreBuilder.Build(world, pos);
		case Species::NO_SPECIES:
			return m_EmptyBuilder.Build(world, pos);
		}

		assert(false && "Unrecognised Species for BuildEntity");
		return INVALID_ENTITY_ID;
	}

private:
	// TODO make array or map (requires clunky shared_ptr casting and maybe enable_shared_from_list as we do components,
	// maybe do when making this not game-specific)
	static EntityBuilder<SpeciesIdentity<Species::PLANT>> m_PlantBuilder;
	static EntityBuilder<SpeciesIdentity<Species::HERBIVORE>> m_HerbivoreBuilder;
	static EntityBuilder<SpeciesIdentity<Species::CARNIVORE>> m_CarnivoreBuilder;
	static EntityBuilder<SpeciesIdentity<Species::NO_SPECIES>> m_EmptyBuilder;

	template<class T>
	static EntityBuilder<T>& GetBuilder()
	{
		switch(T::Value)
		{
			case Species::PLANT: return m_PlantBuilder;
			case Species::HERBIVORE: return m_HerbivoreBuilder;
			case Species::CARNIVORE: return m_CarnivoreBuilder;
			default: return m_EmptyBuilder;
		}
	}
};
