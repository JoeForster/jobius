#pragma once

// TODO_RESOURCE_MANAGEMENT needs full consideration, at least wrap in class
typedef size_t ResourceID;
static const ResourceID ResourceID_Invalid = ~0u;

enum class RenderPass
{
	MAIN,
	DEBUG
};
