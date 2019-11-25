#include "pch.h"
#include "ConvexHull.h"

#include "math/Utility.h"

namespace geom {
	const float ConvexHull::axisTolerance = 0.01f;
	ConvexHull::ConvexHull()
	{
	}

	void ConvexHull::AddVertex(Vector3 vertex)
	{
		auto length = vertex.Length();
		radius = std::max(length, radius);
		vertices.push_back(vertex);
	}
	std::pair<Vector3, Vector3> ConvexHull::BiSupport(Vector3 direction)
	{
		Vector3 min = vertices[0];
		Vector3 max = vertices[0];
		float minDot = INFINITY;
		float maxDot = -INFINITY;
		for (auto & vertex : vertices) {
			auto dot = vertex.Dot(direction);
			if (dot > maxDot) {
				max = vertex;
				maxDot = dot;
			}
			if (dot < minDot) {
				min = vertex;
				minDot = dot;
			}
		}
		return std::make_pair(min,max);
	}
	bool ConvexHull::WithinTolerance(float a, float b)
	{
		return a - axisTolerance <= b && a + axisTolerance >= b;
	}
	Vector3 ConvexHull::Support(Vector3 direction)
	{
		Vector3 max = vertices[0];
		float maxDot = -INFINITY;
		for (auto & vertex : vertices) {
			auto dot = vertex.Dot(direction);
			if (dot > maxDot) {
				max = vertex;
				maxDot = dot;
			}
		}
		return max;
	}
	
	shared_ptr<CollisionVolume> ConvexHull::Transform(M4 matrix)
	{
		auto copy = std::make_shared<ConvexHull>(*this);
		copy->TransformSelf(matrix);
		return std::static_pointer_cast<CollisionVolume>(copy);
	}
	void ConvexHull::TransformSelf(M4 matrix)
	{
		for (auto & vertex : vertices) {
			vertex = matrix * vertex;
		}
	}
}