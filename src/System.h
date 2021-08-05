#pragma once

#include <set>

#include "EntityManager.h"

class System
{
public:
	std::set<EntityID> mEntities;


	//virtual void Init() = 0;
	//virtual void Update(float deltaMs) = 0;
	//virtual void Render() = 0;
	//virtual void Destroy() = 0;

};