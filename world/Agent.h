#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

namespace world {
	struct Agent :
		public ecs::Component
	{
		Agent(float speed, float maxHealth);
		Vector3 Heading;
		float Speed;
		float Health;
		float MaxHealth;
	};
}

