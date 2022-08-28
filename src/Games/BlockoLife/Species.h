#pragma once

#include "Enum.h"

// Get ready for some C++ enum clunk...

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