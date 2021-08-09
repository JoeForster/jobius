#pragma once

#include <vector>

#include "System.h"

class RenderSystem : public System
{
public:

	virtual SystemType GetSystemType() const final override { return SystemType::ST_RENDER; }


/*
	void Init() final override
	{
	
	}

	void Update(float deltaMs) final override
	{

	}

	void Render() final override
	{

	}

	void Destroy() final override
	{

	}
*/
private:

	//std::vector<SpriteRenderer> m_Renderers;
};

