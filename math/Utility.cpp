#include "pch.h"
#include "Utility.h"

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
}