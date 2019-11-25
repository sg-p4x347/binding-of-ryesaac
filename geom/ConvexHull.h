#pragma once
#include "CollisionVolume.h"

namespace geom {
	class ConvexHull : public CollisionVolume
	{
	public:
		ConvexHull();
		void AddVertex(Vector3 vertex);
		virtual std::pair<Vector3, Vector3> BiSupport(Vector3 direction) override;
		bool WithinTolerance(float a, float b);
		// Inherited via CollisionVolume
		virtual Vector3 Support(Vector3 direction) override;
	public:
		std::vector<Vector3> vertices;
		float radius;
		static const float axisTolerance;

		

		// Inherited via CollisionVolume
		virtual shared_ptr<CollisionVolume> Transform(M4 matrix) override;
		virtual void TransformSelf(M4 matrix) override;


	};
}
