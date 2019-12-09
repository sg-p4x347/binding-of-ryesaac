#include "pch.h"
#include "Movement.h"
namespace world {
	Movement::Movement() : Movement::Movement(Vector3::Zero, Vector3::Zero)
	{
	}
	Movement::Movement(Vector3 velocity, Vector3 angularVelocity) : Velocity(velocity), AngularVelocity(angularVelocity)
	{
	}
}