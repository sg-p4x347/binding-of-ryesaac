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
			Normal,
			Boss
		};
		Door(DoorType type = DoorType::Normal, DoorState state = DoorState::Open);
		DoorState State;
		DoorType Type;
	};
}