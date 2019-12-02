#include "pch.h"
#include "Vector2.h"

namespace math {
	Vector2::Vector2() : Vector2::Vector2(0.f, 0.f)
	{
	}

	Vector2::Vector2(float x, float y) : X(x), Y(y)
	{
	}

	float Vector2::Dot(const Vector2& other)
	{
		return X * other.X + Y * other.Y;
	}

	float Vector2::Length()
	{
		return std::sqrt(X * X + Y * Y);
	}

	float Vector2::LengthSquared()
	{
		return X * X + Y * Y;
	}

	void Vector2::Normalize()
	{
		float length = Length();
		if (length > 0.f) {
			X /= length;
			Y /= length;
		}
		else {
			X = 0.f;
			Y = 0.f;
		}
	}

	Vector2 Vector2::Normalized()
	{
		Vector2 copy = *this;
		copy.Normalize();
		return copy;
	}

	Vector2 Vector2::Right()
	{
		return Vector2(Y, -X);
	}

	Vector2 Vector2::Left()
	{
		return Vector2(-Y, X);
	}

	void Vector2::operator+=(const Vector2& b)
	{
		X += b.X;
		Y += b.Y;
	}

	Vector2 Vector2::operator+(const Vector2& b) const
	{
		return Vector2(X + b.X, Y + b.Y);
	}

	void Vector2::operator-=(const Vector2& b)
	{
		X -= b.X;
		Y -= b.Y;
	}

	Vector2 Vector2::operator-() const
	{
		return Vector2(-X, -Y);
	}

	Vector2 Vector2::operator-(const Vector2& b) const
	{
		return Vector2(X - b.X, Y - b.Y);
	}

	void Vector2::operator*=(const float& scalar)
	{
		X *= scalar;
		Y *= scalar;
	}

	Vector2 Vector2::operator*(const float& scalar) const
	{
		return Vector2(X * scalar, Y * scalar);
	}

	void Vector2::operator/=(const float& scalar)
	{
		X /= scalar;
		Y /= scalar;
	}

	Vector2 Vector2::operator/(const float& scalar) const
	{
		return Vector2(X / scalar, Y / scalar);
	}
}