#include "pch.h"
#include "IntVec2.h"

namespace math {
	const IntVec2 IntVec2::Zero = IntVec2(0, 0);

	IntVec2::IntVec2(int x, int y) : X(x), Y(y)
	{
	}
	IntVec2 IntVec2::operator+(const IntVec2& other) const
	{
		return IntVec2(X + other.X, Y + other.Y);
	}
	void IntVec2::operator+=(const IntVec2& other)
	{
		X += other.X;
		Y += other.Y;
	}
	IntVec2 IntVec2::operator-(const IntVec2& other)const
	{
		return IntVec2(X - other.X, Y - other.Y);
	}
	void IntVec2::operator-=(const IntVec2& other)
	{
		X -= other.X;
		Y -= other.Y;
	}
	IntVec2 IntVec2::operator*(const int& scalar)const
	{
		return IntVec2(X * scalar, Y * scalar);
	}
	void IntVec2::operator*=(const int& scalar)
	{
		X *= scalar;
		Y *= scalar;
	}
	IntVec2 IntVec2::operator/(const int& scalar)const
	{
		return IntVec2(X / scalar, Y / scalar);
	}
	void IntVec2::operator/=(const int& scalar)
	{
		X /= scalar;
		Y /= scalar;
	}
	int IntVec2::Dot(const IntVec2& other) const
	{
		return X * other.X + Y * other.Y;
	}
}