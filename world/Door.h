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
		enum class DoorType {
			Normal
		};
		DoorState State;
		DoorType Type;
	};
}