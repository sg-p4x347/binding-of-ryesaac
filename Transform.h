#pragma once
#include "Vector3.h"
#include "Matrix.h"

typedef Matrix<4, 4> M4;
	
M4 CreateTranslation(Vector3 translation);