#pragma once

#include "EntityManager.h"

enum class Species
{
	PLANT,
	HERBIVORE,
	CARNIVORE,

	SPECIES_COUNT
};

struct SpeciesComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_SPECIES; }

	SpeciesComponent() {}
	SpeciesComponent(Species species): m_Species(species) {}

	Species m_Species = Species::PLANT;
};
