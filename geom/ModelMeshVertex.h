#pragma once
#include "math/Vector3.h"
using math::Vector3;

#include "math/Vector2.h"
using math::Vector2;

namespace geom {
	struct ModelMeshVertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 TextureCoordinate;
	};
}
