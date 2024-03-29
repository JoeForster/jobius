#pragma once

#include "EntityBuilder.h"
#include "SDLRenderManager.h"

#include "GridTransformComponent.h"
#include "SpriteComponent.h"
#include "Components/HealthComponent.h"

template<class T>
EntityID EntityBuilder<T>::BuildCommon(World& world, const Vector2i& pos, const int initialHealth, const int maxHealth)
{
	EntityID e = world.CreateEntity();
	world.AddComponent<GridTransformComponent>(e, GridTransformComponent{ pos });
	world.AddComponent<SpriteComponent>(e, m_ResourceID);
	world.AddComponent<SpeciesComponent>(e, T::value);
	world.AddComponent<HealthComponent>(e, { initialHealth, maxHealth });

	return e;
}


EntityID EntityBuilder<SpeciesIdentity<Species::PLANT>>::BuildImpl(World& world, const Vector2i& pos)
{
	constexpr int initialHealth = 1;
	constexpr int maxHealth = 1;

	return BuildCommon(world, pos, initialHealth, maxHealth);
}

EntityID EntityBuilder<SpeciesIdentity<Species::HERBIVORE>>::BuildImpl(World& world, const Vector2i& pos)
{
	constexpr int initialHealth = 10;
	constexpr int maxHealth = 20;

	return BuildCommon(world, pos, initialHealth, maxHealth);
}
	
EntityID EntityBuilder<SpeciesIdentity<Species::CARNIVORE>>::BuildImpl(World& world, const Vector2i& pos)
{
	constexpr int initialHealth = 30;
	constexpr int maxHealth = 40;

	return BuildCommon(world, pos, initialHealth, maxHealth);
}

	
EntityID EntityBuilder<SpeciesIdentity<Species::NO_SPECIES>>::BuildImpl(World& world, const Vector2i& pos)
{
	return BuildCommon(world, pos, 0, 0);
}
