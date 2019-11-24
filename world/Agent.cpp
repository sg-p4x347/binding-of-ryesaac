#include "pch.h"
#include "Agent.h"

namespace world{
	Agent::Agent(float speed, float maxHealth) :
		Speed(speed),
		MaxHealth(maxHealth),
		Health(maxHealth)
	{
	}
}
