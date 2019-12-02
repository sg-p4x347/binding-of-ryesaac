#pragma once
namespace math {
	struct IntVec2
	{
		static const IntVec2 Zero;
		IntVec2(int x = 0, int y = 0);
		IntVec2 operator+(const IntVec2& other) const;
		void operator+=(const IntVec2& other);
		IntVec2 operator-(const IntVec2& other) const;
		void operator-=(const IntVec2& other);
		IntVec2 operator*(const int& scalar) const;
		void operator*=(const int& scalar);
		IntVec2 operator/(const int& scalar) const;
		void operator/=(const int& scalar);

		int Dot(const IntVec2& other) const;
		int X;
		int Y;
	};
}
