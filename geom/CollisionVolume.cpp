#include "pch.h"
#include "CollisionVolume.h"

namespace geom {
	CollisionVolume::CollisionVolume()
	{
	}


	CollisionVolume::~CollisionVolume()
	{
	}
	std::pair<Vector3, Vector3> CollisionVolume::BiSupport(Vector3 direction)
	{
		return std::pair<Vector3, Vector3>(Support(-direction),Support(direction));
	}

	std::pair<float, float> CollisionVolume::Project(Vector3 direction)
	{
		auto points = BiSupport(direction);
		return std::pair<float, float>(points.first.Dot(direction),points.second.Dot(direction));
	}
}