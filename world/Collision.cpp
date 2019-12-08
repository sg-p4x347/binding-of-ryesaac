#include "pch.h"
#include "Collision.h"

namespace world {
	Collision::Collision()
	{
	}
	Collision::Collision(shared_ptr<geom::CollisionVolume> collisionVolume, uint32_t channel) : CollisionVolume(collisionVolume), Channel(channel)
	{
	}
}
