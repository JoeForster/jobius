#pragma once

#include <limits>
#include <cstdint>

// TODO: Templatised vector class
struct Vector2f
{
	float x = 0.0f;
	float y = 0.0f;

	static const Vector2f ZERO;

	const Vector2f operator-(const Vector2f& other) const
	{
		Vector2f v {x - other.x, y - other.y};
		return v;
	}

	const Vector2f operator+(const Vector2f& other) const
	{
		Vector2f v {x + other.x, y + other.y};
		return v;
	}

	const Vector2f operator*(const Vector2f& other) const
	{
		Vector2f v{x * other.x, y * other.y};
		return v;
	}

	Vector2f& operator+=(const Vector2f& other)
	{
		x = x + other.x;
		y = y + other.y;

		return *this;
	}

	Vector2f& operator-=(const Vector2f& other)
	{
		x = x - other.x;
		y = y - other.y;

		return *this;
	}

	Vector2f& operator*=(const float aFloat)
	{
		x *= aFloat;
		y *= aFloat;

		return *this;
	}

	Vector2f& operator/=(const float aFloat)
	{
		x /= aFloat;
		y /= aFloat;

		return *this;
	}
	
	bool operator==(const Vector2f& rVec) const
	{
		return (x == rVec.x && y == rVec.y);
	}
	bool operator!=(const Vector2f& rVec) const
	{
		return (x != rVec.x || y != rVec.y);
	}

	bool IsZero() const
	{
		return (fabsf(x) < std::numeric_limits<float>::epsilon() &&
				fabsf(y) < std::numeric_limits<float>::epsilon());
	}

	float MagnitudeSq() const
	{
		return x*x + y*y;
	}

	float Magnitude() const
	{
		return sqrtf(x*x + y*y);
	}

	float Dist(const Vector2f v) const
	{
		const float dx = x-v.x;
		const float dy = y-v.y;
		return sqrtf(dx*dx + dy*dy);
	}

	float DistSq(const Vector2f v) const
	{
		const float dx = x-v.x;
		const float dy = y-v.y;
		return dx*dx + dy*dy;
	}

	void Normalise()
	{
		float mag = Magnitude();

		if (mag > 0.f)
			*this /= mag;
	}

	void SetMagnitude(float newMag)
	{
		float mag = Magnitude();
		*this *= newMag/mag;
	}

	void LimitMagnitude(float newMag)
	{
		float mag = Magnitude();
		if (mag > newMag)
		{
			*this *= newMag/mag;
		}
	}

	void LimitMagnitude(float minMag, float maxMag)
	{
		float mag = Magnitude();
		if (mag < minMag)
		{
			*this = ZERO;
		}
		else if (mag > maxMag)
		{
			*this *= maxMag/mag;
		}
	}
};


struct Vector2i
{
	int x = 0;
	int y = 0;

	static const Vector2i ZERO;

	const Vector2i operator-(const Vector2i& other) const
	{
		Vector2i v {x - other.x, y - other.y};
		return v;
	}

	const Vector2i operator+(const Vector2i& other) const
	{
		Vector2i v {x + other.x, y + other.y};
		return v;
	}
	
	bool operator==(const Vector2i& rVec) const
	{
		return (x == rVec.x && y == rVec.y);
	}
	bool operator!=(const Vector2i& rVec) const
	{
		return (x != rVec.x || y != rVec.y);
	}

	Vector2i& operator+=(const Vector2i& other)
	{
		x = x + other.x;
		y = y + other.y;

		return *this;
	}
};


struct Vector3f
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	static const Vector3f ZERO;
};

// TODO these don't belong in Vector.h

// TODO rename to Rect2f
struct Rect2f
{
	Vector2f min;
	Vector2f max;

	float GetWidth() const { return max.x-min.x; }
	float GetHeight() const { return max.y-min.y; }

	const bool IsInside(const Vector2f& testPoint)
	{
		return (testPoint.x > min.x && testPoint.x < max.x &&
				testPoint.y > min.y && testPoint.y < max.y);
	}
};

struct Rect2i
{
	Vector2i min;
	Vector2i max;

	int GetWidth() const { return max.x-min.x; }
	int GetHeight() const { return max.y-min.y; }

	const bool IsInside(const Vector2i& testPoint)
	{
		return (testPoint.x > min.x && testPoint.x < max.x &&
				testPoint.y > min.y && testPoint.y < max.y);
	}
};

struct Colour4i
{

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

static constexpr Colour4i RED = { 255, 0, 0, 255 };
static constexpr Colour4i GREEN = { 0, 255, 0, 255 };
static constexpr Colour4i BLUE = { 0, 0, 255, 255 };
static constexpr Colour4i WHITE = { 255, 255, 255, 255 };
