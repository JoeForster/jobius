#pragma once

struct Vector2f
{
	float x;
	float y;

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
};

static constexpr Vector2f VECTOR2F_ZERO { 0.0f, 0.0f };
static constexpr Vector3f VECTOR3F_ZERO { 0.0f, 0.0f, 0.0f };