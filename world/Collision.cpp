#include "pch.h"
#include "Collision.h"

namespace world {
	Collision::Collision()
	{
	}
	Collision::Collision(shared_ptr<geom::CollisionVolume> collisionVolume, bool handlePenetration) : CollisionVolume(collisionVolume), HandlePenetration(handlePenetration)
	{
	}
}
