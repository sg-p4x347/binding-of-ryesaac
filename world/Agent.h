#pragma once
#include "ecs/Component.h"

#include "math/Vector3.h"
using math::Vector3;

namespace world {

	struct Agent :
		public ecs::Component
	{
		enum class AgentFaction {
			Bread,
			Toast
		};

		Agent(AgentFaction faction, float speed, int maxHealth, float attackPeriod, float recoveryPeriod, int damage);

		AgentFaction Faction;

		Vector3 Heading;
		float Speed;
		int Health;
		int MaxHealth;
		
		float AttackPeriod;
		float AttackCooldown;
		bool Attack;

		int Damage;

		float RecoveryPeriod;
		float RecoveryCooldown;
	};
}

