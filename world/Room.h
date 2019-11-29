#pragma once
#include "ecs/EntityRepository.h"

#include "Position.h"
#include "Movement.h"
#include "Collision.h"
#include "Model.h"
#include "Agent.h"
#include "Player.h"
#include "Door.h"

namespace world {
	typedef ecs::EntityRepository<
		Position,
		Movement,
		Collision,
		Model,
		Agent,
		Player,
		Door
	> EntityRepository;

	class Room
	{
	public:
		Room();
		void Update(double elapsed);
		void Render();
		EntityRepository& GetER();
	private:
		void AgentUpdate(double elapsed);
		void AiUpdate(double elapsed);
		void MovementUpdate(double elapsed);
		void CollisionUpdate(double elapsed);
	private:
		EntityRepository ER;
		static const float k_collisionCullRange;
	};
}
