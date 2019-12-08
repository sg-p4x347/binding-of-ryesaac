#pragma once
#include "ecs/Component.h"

#include "LootItem.h"

namespace world {
	struct Player :
		public ecs::Component
	{
		Player();
		map<LootItem,uint32_t> Inventory;
	};
}