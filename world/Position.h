#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

#include "math/Utility.h"
using math::M4;

namespace world {
	struct Position :
		public ecs::Component
	{
		Position();
		Position(Vector3 position, Vector3 rotation);
		M4 GetTransform();

		Vector3 Pos;
		Vector3 Rot;
	};
}
