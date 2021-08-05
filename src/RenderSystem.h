#pragma once

#include <vector>

#include "System.h"

class RenderSystem : public System
{
public:

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

private:

	std::vector<SpriteRenderer> m_Renderers;
};

