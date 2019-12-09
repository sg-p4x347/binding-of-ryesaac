#include "pch.h"
#include "RoomLink.h"
namespace world {
	RoomLink::RoomLink(world::Room&& room, bool locked) : Room(std::move(room)), Locked(locked)
	{
	}
}
