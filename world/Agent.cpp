#include "pch.h"
#include "Agent.h"

namespace world{
	Agent::Agent(AgentFaction faction, float speed, int maxHealth, float attackPeriod, float recoveryPeriod, int damage) :
		Faction(faction),
		Speed(speed),
		MaxHealth(maxHealth),
		Health(maxHealth),
		AttackPeriod(attackPeriod),
		AttackCooldown(0.f),
		Attack(false),
		RecoveryPeriod(recoveryPeriod),
		RecoveryCooldown(0.f),
		Damage(damage)
	{
	}
}
