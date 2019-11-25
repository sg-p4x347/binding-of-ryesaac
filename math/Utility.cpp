#include "pch.h"
#include "Utility.h"

#include <random>

namespace math {
	M4 CreateTranslation(Vector3 translation)
	{
		return M4{
			1.0,0.0,0.0,translation.X,
			0.0,1.0,0.0,translation.Y,
			0.0,0.0,1.0,translation.Z,
			0.0,0.0,0.0,1.0
		};
	}
	M4 CreateRotation(Vector3 axisRotations)
	{
		return CreateRotationX(axisRotations.X) * CreateRotationY(axisRotations.Y) * CreateRotationZ(axisRotations.Z);
	}
	M4 CreateRotationX(float angle)
	{
		return M4 {
			1, 0, 0, 0,
			0, cos(angle), -sin(angle), 0,
			0, sin(angle), cos(angle),  0,
			0, 0, 0, 1
		};
	}
	M4 CreateRotationY(float angle)
	{
		return M4{
			cos(angle), 0, sin(angle), 0,
			0, 1, 0, 0,
			-sin(angle),0, cos(angle), 0,
			0, 0 ,0, 1
		};
	}
	M4 CreateRotationZ(float angle)
	{
		return M4{
			cos(angle), -sin(angle), 0, 0,
			sin(angle), cos(angle), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	}
	double RadToDeg(double radians)
	{
		return radians * 180.0 / PI;
	}
	float RandWithin(float min, float max)
	{
		return min + (max - min) * (float)std::rand() / RAND_MAX;
	}
	int RandWithin(int min, int max)
	{
		return min + std::rand() % (1 + max - min);
	}
	bool Chance(float probability)
	{
		return RandWithin(0.f, 1.f) <= probability;
	}
	bool IntersectionTest(float a0, float a1, float b0, float b1)
	{
		return std::max(a0, a1) > std::min(b0, b1) && std::max(b0, b1) > std::min(a0, a1);
	}
}