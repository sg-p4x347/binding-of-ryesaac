#pragma once
#include "ecs/EntityRepository.h"

#include "Position.h"
#include "Model.h"
#include "Movement.h"
#include "Agent.h"
#include "Player.h"

namespace world {
	typedef ecs::EntityRepository<
		Position,
		Movement,
		Model,
		Agent,
		Player
	> EntityRepository;

	class Room
	{
	public:
		Room();

		void Update(double elapsed);
		EntityRepository& GetER();
		Vector3 GetPosition();
		Vector3 GetSize();
	private:
		void AgentUpdate(double elapsed);
		void AiUpdate(double elapsed);
		void PhysicsUpdate(double elapsed);
	private:
		EntityRepository ER;
		Vector3 m_position;
		Vector3 m_size;
	};
}
