#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

namespace world {
	struct Sweep :
		public ecs::Component
	{
		Sweep();
		vector<Vector3> Waypoints;
		float Duration;
		float Progress;
		int CurrentWaypoint;
	};
}
