#pragma once
#include "Vector2.h"

namespace math {
	struct Vector3
	{
		Vector3();
		Vector3(float x, float y, float z);
		float Length();
		float LengthSquared();
		float X;
		float Y;
		float Z;

		bool operator==(const Vector3& other) const;

		void operator+=(const Vector3& b);
		Vector3 operator+(const Vector3& b) const;
		void operator-=(const Vector3& b);
		Vector3 operator-() const;
		Vector3 operator-(const Vector3& b) const;

		// float scalar operations
		void operator*=(const float& scalar);
		Vector3 operator*(const float& scalar) const;
		void operator/=(const float& scalar);
		Vector3 operator/(const float& scalar) const;

		// double scalar operations
		void operator*=(const double& scalar);
		Vector3 operator*(const double& scalar) const;
		void operator/=(const double& scalar);
		Vector3 operator/(const double& scalar) const;

		// int scalar operations
		void operator*=(const int& scalar);
		Vector3 operator*(const int& scalar) const;
		void operator/=(const int& scalar);
		Vector3 operator/(const int& scalar) const;
		
		void Normalize();
		Vector3 Normalized() const;

		float Dot(const Vector3& other) const;
		Vector3 Cross(const Vector3& other) const;

		static const Vector3 Zero;
		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
	};
}