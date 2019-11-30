#include "pch.h"
#include "Room.h"

#include "geom/ModelRepository.h"
using geom::ModelRepository;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "geom/CollisionUtil.h"

#include "geom/Sphere.h"

#include <GL/glut.h>

namespace world {
	const float Room::k_collisionCullRange = 1.5f;
	Room::Room()
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

		for (auto& entity : ER.GetIterator<Model, Position>()) {
			Position& position = entity.GetBitmap<Position>();
			Model& modelComp = entity.GetBitmap<Model>();
			if (modelComp.ModelPtr) {

				glPushMatrix();
				glBindTexture(GL_TEXTURE_2D,TextureRepository::GetID(modelComp.ModelPtr->Name));
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
	}

	EntityRepository& Room::GetER()
	{
		return ER;
	}

	void Room::AgentUpdate(double elapsed)
	{
		set<ecs::EntityID> dead;
		for (auto& entity : ER.GetIterator<Agent, Movement, Collision, Position>()) {
			auto& agent = entity.GetBitmap<Agent>();
			auto& movement = entity.GetBitmap<Movement>();
			auto& collision = entity.GetBitmap<Collision>();
			auto& position = entity.GetBitmap<Position>();

			// Convert agent heading into a velocity
			agent.Heading.Normalize();
			movement.Velocity = agent.Heading * agent.Speed;

			// Update attack cooldown
			agent.AttackCooldown = std::max(0.0, agent.AttackCooldown - elapsed);
			
			// Update recovery cooldown
			agent.RecoveryCooldown = std::max(0.0, agent.RecoveryCooldown - elapsed);

			if (agent.Attack && !agent.AttackCooldown) {
				shared_ptr<geom::Model> projectileModel;
				switch (agent.Faction) {
				case Agent::AgentFaction::Bread:
					projectileModel = ModelRepository::GetBitmap("butter");
					break;
				case Agent::AgentFaction::Toast:
					projectileModel = ModelRepository::GetBitmap("butter");
					break;
				}
				Agent projectile = Agent(agent.Faction, 16.f, 0, 0.f, 0.f, 1);
				projectile.Heading = Vector3(std::cos(position.Rot.Y), 0.f, std::sin(position.Rot.Y));
				ER.CreateEntity(
					Position(position.Pos + projectile.Heading, position.Rot),
					projectile,
					Movement(),
					Collision(std::make_shared<geom::Sphere>(Vector3::Zero, 0.25), false),
					Model(projectileModel)
				);
				agent.AttackCooldown = agent.AttackPeriod;
			}
			if (agent.MaxHealth > 0) {
				if (agent.Health <= 0) {
					dead.insert(agent.ID);
				}
				else {
					// Apply damage if not in recovery (temporary invincibility after taking damage)
					if (!agent.RecoveryCooldown && !collision.Contacts.empty()) {
						for (auto& other : ER.GetIterator<Agent>()) {
							auto& otherAgent = other.GetBitmap<Agent>();
							// check to see if other is a different faction and shows up in our contact list
							if (otherAgent.Faction != agent.Faction) {
								for (auto& contact : collision.Contacts) {
									if (contact.Collider == otherAgent.ID) {
										// Apply other agent's damage to our health
										agent.Health -= otherAgent.Damage;
										// Start recovery cooldown
										agent.RecoveryCooldown = agent.RecoveryPeriod;
									}
								}
							}
						}
					}
				}
			}
			else {
				if (!collision.Contacts.empty()) {
					dead.insert(agent.ID);
				}
			}
		}
		// Remove all dead agents
		for (auto& id : dead)
			ER.Remove(id);
	}

	void Room::AiUpdate(double elapsed)
	{
	}

	void Room::MovementUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Movement, Position>()) {
			auto& movement = entity.GetBitmap<Movement>();
			auto& position = entity.GetBitmap<Position>();

			position.Pos += movement.Velocity * elapsed;
		}
	}
	void Room::CollisionUpdate(double elapsed)
	{
		for (auto& dynamicCollider : ER.GetIterator<Movement,Collision, Position>()) {
			auto& movement = dynamicCollider.GetBitmap<Movement>();
			auto& dynamicCollision = dynamicCollider.GetBitmap<Collision>();
			auto& dynamicPosition = dynamicCollider.GetBitmap<Position>();
			dynamicCollision.Contacts.clear();
			auto dynamicCollisionVolume = dynamicCollision.CollisionVolume->Transform(dynamicPosition.GetTransform());
			for (auto& staticCollider : ER.GetIterator<Collision, Position>()) {
				auto& staticCollision = staticCollider.GetBitmap<Collision>();
				auto& staticPosition = staticCollider.GetBitmap<Position>();
				if (staticCollision.ID != dynamicCollision.ID && (staticPosition.Pos - dynamicPosition.Pos).LengthSquared() < k_collisionCullRange * k_collisionCullRange) {
					staticCollision.Contacts.clear();
					auto staticCollisionVolume = staticCollision.CollisionVolume->Transform(staticPosition.GetTransform());
					// Use GJK to test if an intersection exists
					geom::GjkIntersection intersection;
					if (geom::GJK(*dynamicCollisionVolume, *staticCollisionVolume, intersection)) {
						// Use EPA to get the contact details
						Collision::Contact contact;
						if (geom::EPA(*dynamicCollisionVolume, *staticCollisionVolume, intersection, contact)) {
							// Immediately correct the position in the X-Z plane
							if (dynamicCollision.HandlePenetration) {
								dynamicPosition.Pos.X += contact.Normal.X * contact.PenetrationDepth;
								dynamicPosition.Pos.Z += contact.Normal.Z * contact.PenetrationDepth;
								// Update collision volume
								dynamicCollisionVolume = dynamicCollision.CollisionVolume->Transform(dynamicPosition.GetTransform());
							}
							// Register contacts
							contact.Collider = staticCollision.ID;
							dynamicCollision.Contacts.push_back(contact);

						}
					}
				}
			}
		}
	}
}