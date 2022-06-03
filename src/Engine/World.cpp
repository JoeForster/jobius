#include "World.h"

#include "Vector.h"

// TODO configurable in a subclass or somewhere else?
const Rect2D& World::GetBounds() const
{
	static auto s_DefaultBounds = Rect2D{ Vector2f{0, 0}, Vector2f{1000, 700} };
	return s_DefaultBounds;
}