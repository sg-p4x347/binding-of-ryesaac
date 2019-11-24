#pragma once
#include "ecs/Component.h"
namespace world {
	struct Player :
		public ecs::Component
	{
		Player();
	};
}