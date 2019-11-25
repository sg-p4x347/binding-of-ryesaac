#pragma once
#include "pch.h"

#include "ecs/Component.h"

#include "geom/CollisionVolume.h"

namespace world {
	struct Collision :
		public ecs::Component
	{
		Collision();
		Collision(shared_ptr<geom::CollisionVolume> collisionVolume);
		struct Contact {
			ecs::EntityID Collider;
			Vector3 Point;
			Vector3 Normal;
			float PenetrationDepth;
		};
		vector<Contact> Contacts;
		shared_ptr<geom::CollisionVolume> CollisionVolume;
	};
}
