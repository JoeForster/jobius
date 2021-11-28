#pragma once

#include <limits>

struct Vector2f
{
	float x = 0.0f;
	float y = 0.0f;

	static const Vector2f ZERO;

	//Vector2f(float initX, float initY)
	//: x(initX), y(initY) {}

	const Vector2f Vector2f::operator-(const Vector2f& other) const
	{
		Vector2f v {x - other.x, y - other.y};
		return v;
	}

	const Vector2f Vector2f::operator+(const Vector2f& other) const
	{
		Vector2f v {x + other.x, y + other.y};
		return v;
	}


	const Vector2f Vector2f::operator*(const Vector2f& other) const
	{
		Vector2f v{x * other.x, y * other.y};
		return v;
	}

	Vector2f& Vector2f::operator+=(const Vector2f& other)
	{
		x = x + other.x;
		y = y + other.y;

		return *this;
	}

	Vector2f& Vector2f::operator-=(const Vector2f& other)
	{
		x = x - other.x;
		y = y - other.y;

		return *this;
	}

	Vector2f& Vector2f::operator*=(const float aFloat)
	{
		x *= aFloat;
		y *= aFloat;

		return *this;
	}

	Vector2f& Vector2f::operator/=(const float aFloat)
	{
		x /= aFloat;
		y /= aFloat;

		return *this;
	}

	bool Vector2f::operator>(const Vector2f& rVec) const
	{
		return (x > rVec.x && y > rVec.y);
	}
	bool Vector2f::operator>=(const Vector2f& rVec) const
	{
		return (x >= rVec.x && y >= rVec.y);
	}

	bool Vector2f::operator<(const Vector2f& rVec) const
	{
		return (x < rVec.x && y < rVec.y);
	}
	bool Vector2f::operator<=(const Vector2f& rVec) const
	{
		return (x <= rVec.x && y <= rVec.y);
	}

	bool IsZero() const
	{
		return (fabsf(x) < std::numeric_limits<float>::epsilon() &&
				fabsf(y) < std::numeric_limits<float>::epsilon());
	}

	float Vector2f::MagnitudeSq() const
	{
		return x*x + y*y;
	}

	float Vector2f::Magnitude() const
	{
		return sqrtf(x*x + y*y);
	}

	void Vector2f::Normalise()
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
};

struct Vector3f
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	static const Vector3f ZERO;
};

struct Rect2D
{
	Vector2f min;
	Vector2f max;

	const bool IsInside(const Vector2f& testPoint)
	{
		return (testPoint > min && testPoint < max);
	}
};