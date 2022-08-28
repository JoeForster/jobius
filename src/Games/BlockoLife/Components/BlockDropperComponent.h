#pragma once

#include "EntityManager.h"
#include "Vector.h"

#include "Species.h"

struct BlockDropperComponent
{
	static constexpr ComponentType GetComponentType() { return ComponentType::CT_BL_DROPPER; }

	BlockDropperComponent() {}
	BlockDropperComponent(Species species): m_Species(species) {}

	Species m_Species = Species::PLANT;
};
