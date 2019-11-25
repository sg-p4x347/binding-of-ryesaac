#pragma once

#include "math/Vector3.h"
using math::Vector3;

#include "math/Utility.h"
using math::M4;

/*
Base class for specific collision volumes
*/
namespace geom {
	class CollisionVolume
	{
	public:
		CollisionVolume();
		~CollisionVolume();
		// Returns the furthest point within the volume in the given direction
		virtual Vector3 Support(Vector3 direction) = 0;
		// Returns the furthest points (backwards,forwards) in the given direction
		virtual std::pair<Vector3, Vector3> BiSupport(Vector3 direction);
		// Returns a new concrete volume instance transformed by the given matrix
		virtual shared_ptr<CollisionVolume> Transform(M4 matrix) = 0;
		// Transforms this volume by the given matrix
		virtual void TransformSelf(M4 matrix) = 0;
		// Returns the min and max dot products in the given direction
		virtual std::pair<float, float> Project(Vector3 direction);
	};
}
