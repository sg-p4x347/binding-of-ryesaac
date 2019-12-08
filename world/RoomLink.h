#pragma once
#include "Room.h"

namespace world {
	struct RoomLink
	{
		RoomLink(Room&& room, bool locked);
		Room Room;
		bool Locked;
	};
}
