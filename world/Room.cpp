#include "pch.h"
#include "Room.h"

#include "geom/CollisionUtil.h"

#include <GL/glut.h>

namespace world {
	Room::Room(Vector3 position, Vector3 size) : m_position(position), m_size(size)
	{
	}

	void Room::Update(double elapsed)
	{
		AiUpdate(elapsed);
		AgentUpdate(elapsed);
		MovementUpdate(elapsed);
		CollisionUpdate(elapsed);
	}

	void Room::Render()
	{
		glTranslatef(m_position.X, m_position.Y, m_position.Z);

		for (auto& entity : ER.GetIterator<Model, Position>()) {
			glPushMatrix();
			Position& position = entity.Get<Position>();
			Model& modelComp = entity.Get<Model>();

			glTranslatef(position.Pos.X, position.Pos.Y, position.Pos.Z);
			glRotatef(math::RadToDeg(position.Rot.X), 1.f, 0.f, 0.f);
			glRotatef(math::RadToDeg(position.Rot.Y), 0.f, 1.f, 0.f);
			glRotatef(math::RadToDeg(position.Rot.Z), 0.f, 0.f, 1.f);


			geom::Model& model = *modelComp.ModelPtr;
			for (auto& mesh : model.Meshes) {

				glBegin(GL_TRIANGLES);
				for (auto& index : mesh.Indices) {
					geom::ModelMeshVertex& vertex = mesh.Vertices[index];
					glNormal3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
					glTexCoord2f(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y);
					glVertex3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);

				}
				glEnd();
			}


			glPopMatrix();
		}
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

	bool Room::Contains(Vector3 point)
	{
		return point.X >= m_position.X
			&& point.X <= m_position.X + m_size.X
			&& point.Y >= m_position.Y
			&& point.Y <= m_position.Y + m_size.Y
			&& point.Z >= m_position.Z
			&& point.Z <= m_position.Z + m_size.Z;

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

	void Room::MovementUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Movement, Position>()) {
			auto& movement = entity.Get<Movement>();
			auto& position = entity.Get<Position>();

			position.Pos += movement.Velocity * elapsed;
		}
	}
	void Room::CollisionUpdate(double elapsed)
	{
		for (auto& dynamicCollider : ER.GetIterator<Movement,Collision, Position>()) {
			auto& movement = dynamicCollider.Get<Movement>();
			auto& dynamicCollision = dynamicCollider.Get<Collision>();
			auto& dynamicPosition = dynamicCollider.Get<Position>();

			auto dynamicCollisionVolume = dynamicCollision.CollisionVolume->Transform(dynamicPosition.GetTransform());
			for (auto& staticCollider : ER.GetIterator<Collision, Position>()) {
				auto& staticCollision = staticCollider.Get<Collision>();
				auto& staticPosition = staticCollider.Get<Position>();

				auto staticCollisionVolume = staticCollision.CollisionVolume->Transform(staticPosition.GetTransform());
				// Use GJK to test if an intersection exists
				geom::GjkIntersection intersection;
				if (geom::GJK(*dynamicCollisionVolume, *staticCollisionVolume, intersection)) {
					// Use EPA to get the contact details
					Collision::Contact contact;
					if (geom::EPA(*dynamicCollisionVolume, *staticCollisionVolume, intersection, contact)) {
						// Immediately correct the position in the X-Z plane
						dynamicPosition.Pos.X += contact.Normal.X * contact.PenetrationDepth;
						dynamicPosition.Pos.Z += contact.Normal.Z * contact.PenetrationDepth;
						// Update collision volume
						dynamicCollisionVolume = dynamicCollision.CollisionVolume->Transform(dynamicPosition.GetTransform());
					}
				}
			}
		}
	}
}