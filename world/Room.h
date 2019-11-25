#pragma once
#include "ecs/EntityRepository.h"

#include "Position.h"
#include "Movement.h"
#include "Collision.h"
#include "Model.h"
#include "Agent.h"
#include "Player.h"

namespace world {
	typedef ecs::EntityRepository<
		Position,
		Movement,
		Collision,
		Model,
		Agent,
		Player
	> EntityRepository;

	class Room
	{
	public:
		Room(Vector3 position, Vector3 size);
		void Update(double elapsed);
		void Render();
		EntityRepository& GetER();
		Vector3 GetPosition();
		Vector3 GetSize();
		// Inclusive of edges
		bool Contains(Vector3 point);
	private:
		void AgentUpdate(double elapsed);
		void AiUpdate(double elapsed);
		void MovementUpdate(double elapsed);
		void CollisionUpdate(double elapsed);
	private:
		EntityRepository ER;
		Vector3 m_position;
		Vector3 m_size;
	};
}
