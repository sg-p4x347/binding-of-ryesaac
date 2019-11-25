#include "pch.h"
#include "Sphere.h"

namespace geom {
	Sphere::Sphere(Vector3 c,float r) : center(c),radius(r)
	{
	}


	Sphere::~Sphere()
	{
	}

	
	Vector3 Sphere::Support(Vector3 direction)
	{
		return center + direction * radius;
	}

	shared_ptr<CollisionVolume> Sphere::Transform(M4 matrix)
	{
		return std::make_shared<Sphere>(matrix * center,radius);
	}
	void Sphere::TransformSelf(M4 matrix)
	{
		center = matrix * center;
	}
}