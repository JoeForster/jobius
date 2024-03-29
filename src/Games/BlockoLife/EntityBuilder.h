#pragma once

#include "World.h"
#include "EntityManager.h"
#include "Vector.h"
#include "SDLRenderManager.h"

#include "Components/SpeciesComponent.h"

class IEntityBuilder : public std::enable_shared_from_this<World>
{
public:
	virtual ResourceID LoadResources(SDLRenderManager& renderMan) = 0;
	virtual EntityID Build(World& world, const Vector2i& pos) = 0;
};

// TODO_ENTITY_BUILDER out some metaprogramming here; not final (templatise the class instead and create resource here?)
template<class T>
class EntityBuilder : public IEntityBuilder
{
public:
	static ResourceID LoadCreatureSprite(SDLRenderManager& renderMan, Species species)
	{
		static constexpr const char* resourcePaths[SpeciesCount] {
			"assets/sprites/plant_block_32.png",
			"assets/sprites/herbivore_32.png",
			"assets/sprites/carnivore_32.png"
		};

		return renderMan.LoadTexture(resourcePaths[to_underlying(species)]);
	}

	ResourceID LoadResources(SDLRenderManager& renderMan) final
	{
		m_ResourceID = LoadCreatureSprite(renderMan, T::value);
		assert(m_ResourceID != ResourceID_Invalid);
		return m_ResourceID;
	}
	
	EntityID Build(World& world, const Vector2i& pos) final { return BuildImpl(world, pos); }

private:
	EntityID BuildImpl(World& world, const Vector2i& pos) { assert(false); return BuildCommon(world, pos, 1, 1); }

	EntityID BuildCommon(World& world, const Vector2i& pos, const int initialHealth, const int maxHealth);

	ResourceID m_ResourceID = ResourceID_Invalid;

};

template<>
EntityID EntityBuilder<SpeciesIdentity<Species::PLANT>>::BuildImpl(World& world, const Vector2i& pos);

template<>
EntityID EntityBuilder<SpeciesIdentity<Species::HERBIVORE>>::BuildImpl(World& world, const Vector2i& pos);

template<>
EntityID EntityBuilder<SpeciesIdentity<Species::CARNIVORE>>::BuildImpl(World& world, const Vector2i& pos);

template<>
EntityID EntityBuilder<SpeciesIdentity<Species::NO_SPECIES>>::BuildImpl(World& world, const Vector2i& pos);
