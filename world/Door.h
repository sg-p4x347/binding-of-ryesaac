#pragma once
#include "ecs/Component.h"

namespace world {
	struct Door :
		public ecs::Component
	{
		enum class DoorState {
			Open,
			Closed,
			Locked
		};
		DoorState State;
	};
}