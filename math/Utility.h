#pragma once
#include "Vector3.h"
#include "math/Matrix.h"

namespace math {

	//----------------------------------------------------------------
	// Geometry
	static const float PI = 3.14159265359;
	static const float TWO_PI = PI * 2.0;

	typedef Matrix<4, 4> M4;

	M4 CreateTranslation(Vector3 translation);
	M4 CreateRotation(Vector3 axisRotations);
	M4 CreateRotationX(float angle);
	M4 CreateRotationY(float angle);
	M4 CreateRotationZ(float angle);
	double RadToDeg(double radians);

	//----------------------------------------------------------------
	// Randomness

	// inclusive
	float RandWithin(float min = 0.f, float max = 1.f);
	// inclusive
	int RandWithin(int min, int max);

	bool Chance(float probability);
	// Performs a Fisher-Yates in-place shuffle
	template<typename T>
	void Shuffle(T& items) {
		for (int i = 0; i < items.size() - 1; i++) {
			int j = RandWithin(i, items.size() - 1);
			std::swap(*(items.begin() + i), *(items.begin() + j));
		}
	}
	//----------------------------------------------------------------
	// Intersection

	// non-inclusive
	bool IntersectionTest(float a0, float a1, float b0, float b1);


}