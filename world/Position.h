#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

namespace world {
	struct Position :
		public ecs::Component
	{
		Position();
		Position(Vector3 position);
		Vector3 Pos;
	};
}
