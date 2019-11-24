#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

struct Movement :
	public ecs::Component
{
	Vector3 Velocity;
};

