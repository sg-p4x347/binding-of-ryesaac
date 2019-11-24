#include "pch.h"
#include "Room.h"

namespace world {
	Room::Room()
	{
	}

	void Room::Update(double elapsed)
	{
		AiUpdate(elapsed);
		AgentUpdate(elapsed);
		PhysicsUpdate(elapsed);
	}

	EntityRepository& Room::GetER()
	{
		return ER;
	}

	Vector3 Room::GetPosition()
	{
		return m_position;
	}

	Vector3 Room::GetSize()
	{
		return m_size;
	}


	void Room::AgentUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Agent, Movement>()) {
			auto& agent = entity.Get<Agent>();
			auto& movement = entity.Get<Movement>();

			// Convert agent heading into a velocity
			agent.Heading.Normalize();
			movement.Velocity = agent.Heading * agent.Speed;

		}
	}

	void Room::AiUpdate(double elapsed)
	{
	}

	void Room::PhysicsUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Movement, Position>()) {
			auto& movement = entity.Get<Movement>();
			auto& position = entity.Get<Position>();

			position.Pos += movement.Velocity * elapsed;
		}
	}
}