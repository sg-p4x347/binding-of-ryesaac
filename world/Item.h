#pragma once
#include "ecs/Component.h"

#include "LootItem.h"

namespace world {
	struct Item :
		public ecs::Component
	{
		Item(LootItem type);

		LootItem Type;

	};
}

