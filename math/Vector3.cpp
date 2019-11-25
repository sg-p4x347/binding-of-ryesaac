#include "pch.h"
#include "Vector3.h"

namespace math {
	const Vector3 Vector3::Zero = Vector3(0.f, 0.f, 0.f);
	const Vector3 Vector3::UnitX = Vector3(1.f, 0.f, 0.f);
	const Vector3 Vector3::UnitY = Vector3(0.f, 1.f, 0.f);
	const Vector3 Vector3::UnitZ = Vector3(0.f, 0.f, 1.f);

	Vector3::Vector3() : Vector3::Vector3(0.f, 0.f, 0.f)
	{
	}

	Vector3::Vector3(float x, float y, float z) : X(x), Y(y), Z(z)
	{
	}

	float Vector3::Length()
	{
		return std::sqrt(X * X + Y * Y + Z * Z);
	}

	float Vector3::LengthSquared()
	{
		return X * X + Y * Y + Z * Z;
	}

	bool Vector3::operator==(const Vector3& other) const
	{
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	void Vector3::operator+=(const Vector3& b)
	{
		X += b.X;
		Y += b.Y;
		Z += b.Z;
	}

	Vector3 Vector3::operator+(const Vector3& b) const
	{
		return Vector3(X + b.X, Y + b.Y, Z + b.Z);
	}

	void Vector3::operator-=(const Vector3& b)
	{
		X -= b.X;
		Y -= b.Y;
		Z -= b.Z;
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-X, -Y, -Z);
	}

	Vector3 Vector3::operator-(const Vector3& b) const
	{
		return Vector3(X - b.X, Y - b.Y, Z - b.Z);;
	}

	void Vector3::operator*=(const float& scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	Vector3 Vector3::operator*(const float& scalar) const
	{
		return Vector3(X * scalar, Y * scalar, Z * scalar);
	}


	void Vector3::operator/=(const float& scalar)
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}

	Vector3 Vector3::operator/(const float& scalar) const
	{
		return Vector3(X / scalar, Y / scalar, Z / scalar);
	}

	void Vector3::operator*=(const double& scalar)
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}

	Vector3 Vector3::operator*(const double& scalar) const
	{
		return Vector3(X * scalar, Y * scalar, Z * scalar);
	}

	void Vector3::operator/=(const double& scalar)
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}

	Vector3 Vector3::operator/(const double& scalar) const
	{
		return Vector3(X / scalar, Y / scalar, Z / scalar);
	}

	void Vector3::operator*=(const int& scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	Vector3 Vector3::operator*(const int& scalar) const
	{
		return Vector3(X * scalar, Y * scalar, Z * scalar);
	}
	void Vector3::operator/=(const int& scalar)
	{
		X /= scalar;
		Y /= scalar;
		Z /= scalar;
	}
	Vector3 Vector3::operator/(const int& scalar) const
	{
		return Vector3(X / scalar, Y / scalar, Z / scalar);
	}
	void Vector3::Normalize()
	{
		float length = Length();
		if (length == 0.f) {
			X = 0.f;
			Y = 0.f;
			Z = 0.f;
		}
		else {
			X /= length;
			Y /= length;
			Z /= length;
		}
	}
	Vector3 Vector3::Normalized() const
	{
		Vector3 normalized = *this;
		normalized.Normalize();
		return normalized;
	}
	float Vector3::Dot(const Vector3& other) const
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}
	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return Vector3(
			(Y * other.Z) - (Z * other.Y),
			(Z * other.X) - (X * other.Z),
			(X * other.Y) - (Y * other.X)
		);
	}
}