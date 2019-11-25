#pragma once
#include "CollisionVolume.h"
namespace geom {
	class Sphere :
		public CollisionVolume
	{
	public:
		Sphere(Vector3 c = Vector3::Zero,float r = 1.f);
		~Sphere();

		float radius;
		Vector3 center;
		// Inherited via CollisionVolume
		virtual Vector3 Support(Vector3 direction) override;
		virtual shared_ptr<CollisionVolume> Transform(M4 matrix) override;
		virtual void TransformSelf(M4 matrix) override;
	};
}