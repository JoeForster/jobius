#pragma once

#include "EntityManager.h"

#include "Species.h"



struct SpeciesComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_SPECIES; }

	SpeciesComponent() {}
	SpeciesComponent(Species species): m_Species(species) {}

	Species m_Species = Species::PLANT;
};
