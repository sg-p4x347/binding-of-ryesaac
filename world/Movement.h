#pragma once
#include "pch.h"
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

namespace world {
	struct Movement :
		public ecs::Component
	{
		Movement();
		Movement(Vector3 velocity, Vector3 angularVelocity);
		Vector3 Velocity;
		Vector3 AngularVelocity;
	};
}
