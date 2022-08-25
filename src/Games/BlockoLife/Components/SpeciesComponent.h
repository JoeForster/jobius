#pragma once

#include "EntityManager.h"

// Get ready for some C++ enum clunk...

// TODO ENUM HELPERS make separate
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

enum class Species
{
	PLANT,
	HERBIVORE,
	CARNIVORE,

	SPECIES_COUNT
};

template<Species X>
struct SpeciesIdentity {
	static constexpr Species value = X;
};
static_assert(Species::PLANT == SpeciesIdentity<Species::PLANT>::value);

constexpr size_t SpeciesCount = to_underlying(Species::SPECIES_COUNT);

struct SpeciesComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_SPECIES; }

	SpeciesComponent() {}
	SpeciesComponent(Species species): m_Species(species) {}

	Species m_Species = Species::PLANT;
};
