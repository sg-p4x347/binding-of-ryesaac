#pragma once
#include "Vector3.h"
#include "math/Matrix.h"

namespace math {
	typedef Matrix<4, 4> M4;

	M4 CreateTranslation(Vector3 translation);
}