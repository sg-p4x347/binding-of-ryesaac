#include "pch.h"
#include "Room.h"

#include "geom/ModelRepository.h"
using geom::ModelRepository;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "geom/CollisionUtil.h"

#include "geom/Sphere.h"

#include "game/Game.h"
using game::Game;
#include "game/MultimediaPlayer.h"
using game::MultimediaPlayer;

#include <GL/glut.h>

namespace world {
	const float Room::k_collisionCullRange = 1.5f;
	Room::Room(RoomType type, Vector3 center) : m_center(center), m_type(type), m_inCombat(false)
	{
	}

	void Room::Update(double elapsed)
	{
		AiUpdate(elapsed);
		AgentUpdate(elapsed);
		MovementUpdate(elapsed);
		CollisionUpdate(elapsed);
		PlayerLocationUpdate();
		DoorUpdate(elapsed);
		CombatUpdate(elapsed);
		ItemUpdate(elapsed);
		DeferredUpdate(elapsed);
	}

	void Room::Render()
	{

		for (auto& entity : ER.GetIterator<Model, Position>()) {
			Position& position = entity.Get<Position>();
			Model& modelComp = entity.Get<Model>();
			if (!modelComp.Hidden && modelComp.ModelPtr) {

				glPushMatrix();
				glEnable(GL_TEXTURE_2D);
				
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
				glBindTexture(GL_TEXTURE_2D, 0);

				glPopMatrix();
			}
		}
	}

	EntityRepository& Room::GetER()
	{
		return ER;
	}

	Room::RoomType Room::GetType()
	{
		return m_type;
	}

	void Room::AddLoot(LootItem item)
	{
		m_loot.push_back(item);
	}

	void Room::DropLoot()
	{
		for (auto& loot : m_loot) {
			string modelName = "";
			switch (loot) {
			case LootItem::Key: modelName = "key"; break;
			}
			m_deferredTasks.push_back([=] {
				ER.CreateEntity(
					Position(m_center, Vector3::Zero),
					Movement(Vector3::Zero,Vector3(0.f,1.f,0.f)),
					Model(ModelRepository::Get(modelName)),
					Collision(std::make_shared<geom::Sphere>(Vector3::Zero, 0.25f)),
					Item(loot)
				);
			});
		}
		// Consume the loot
		m_loot.clear();
	}

	void Room::AgentUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Agent, Model, Movement, Collision, Position>()) {
			auto& agent = entity.Get<Agent>();
			auto& model = entity.Get<Model>();
			auto& movement = entity.Get<Movement>();
			auto& collision = entity.Get<Collision>();
			auto& position = entity.Get<Position>();

			// Convert agent heading into a velocity
			agent.Heading.Normalize();
			movement.Velocity = agent.Heading * agent.Speed;

			// Update attack cooldown
			agent.AttackCooldown = max(0.0, agent.AttackCooldown - elapsed);
			
			// Update recovery cooldown
			agent.RecoveryCooldown = max(0.0, agent.RecoveryCooldown - elapsed);
			// Strobe the model while in cooldown
			if (agent.RecoveryCooldown) {
				static const float k_recoveryFlashPeriod = 0.25f;
				model.Hidden = std::fmodf(agent.RecoveryCooldown, k_recoveryFlashPeriod) <= k_recoveryFlashPeriod * 0.5f;
			}
			else {
				model.Hidden = false;
			}

			if (agent.Attack && !agent.AttackCooldown) {
				shared_ptr<geom::Model> projectileModel;
				switch (agent.Faction) {
				case Agent::AgentFaction::Bread:
					projectileModel = ModelRepository::Get("butter");
					break;
				case Agent::AgentFaction::Toast:
					projectileModel = ModelRepository::Get("butter");
					break;
				}
				
				m_deferredTasks.push_back([=] {
					Agent projectile = Agent(agent.Faction, 8.f, 0, 0.f, 0.f, 1);
					projectile.Heading = Vector3(std::cos(-position.Rot.Y), 0.f, std::sin(-position.Rot.Y));
					ER.CreateEntity(
						Position(position),
						projectile,
						Movement(Vector3::Zero,Vector3(0.f,math::TWO_PI * 5.f,0.f)),
						Collision(std::make_shared<geom::Sphere>(Vector3::Zero, 0.25), (uint32_t)CollisionChannel::Projectile),
						Model(projectileModel)
					);
				});
				agent.AttackCooldown = agent.AttackPeriod;
			}
			if (agent.MaxHealth > 0) {
				if (agent.Health <= 0) {
					m_deferredTasks.push_back([=] {
						ER.Remove(agent.ID);
					});
				}
				else {
					// Apply damage if not in recovery (temporary invincibility after taking damage)
					if (!agent.RecoveryCooldown && !collision.Contacts.empty()) {
						for (auto& other : ER.GetIterator<Agent>()) {
							auto& otherAgent = other.Get<Agent>();
							// check to see if other is a different faction and shows up in our contact list
							if (otherAgent.Faction != agent.Faction && collision.Contacts.count(otherAgent.ID)) {
								// Apply other agent's damage to our health
								agent.Health -= otherAgent.Damage;
								// Start recovery cooldown
								agent.RecoveryCooldown = agent.RecoveryPeriod;
							}
						}
					}
				}
			}
			else {
				if (!collision.Contacts.empty()) {
					m_deferredTasks.push_back([=] {
						ER.Remove(agent.ID);
					});
				}
			}
		}
	}

	void Room::AiUpdate(double elapsed)
	{
		for (auto& playerEntity : ER.GetIterator<Player, Agent, Position>()) {
			auto& player = playerEntity.Get<Agent>();
			auto& playerPos = playerEntity.Get<Position>();

			for (auto& enemyEntity : ER.GetIterator<Agent, Position>()) {
				auto& enemy = enemyEntity.Get<Agent>();
				auto& enemyPos = enemyEntity.Get<Position>();
				if (enemy.Faction != player.Faction) {
					enemy.Heading = playerPos.Pos - enemyPos.Pos;
					enemyPos.Rot.Y = -std::atan2f(enemy.Heading.Z, enemy.Heading.X);
				}
			}
		}
	}

	void Room::MovementUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Movement, Position>()) {
			auto& movement = entity.Get<Movement>();
			auto& position = entity.Get<Position>();

			position.Pos += movement.Velocity * elapsed;
			position.Rot += movement.AngularVelocity * elapsed;
			// Wrap rotations around 2 pi
			position.Rot.X = std::fmodf(position.Rot.X, math::TWO_PI);
			position.Rot.Y = std::fmodf(position.Rot.Y, math::TWO_PI);
			position.Rot.Z = std::fmodf(position.Rot.Z, math::TWO_PI);
		}
	}
	void Room::CollisionUpdate(double elapsed)
	{
		// Clear contacts
		for (auto& collider : ER.GetIterator<Collision>()) {
			collider.Get<Collision>().Contacts.clear();
		}

		for (auto& dynamicCollider : ER.GetIterator<Movement,Collision, Position>()) {
			auto& movement = dynamicCollider.Get<Movement>();
			auto& dynamicCollision = dynamicCollider.Get<Collision>();
			auto& dynamicPosition = dynamicCollider.Get<Position>();
			auto dynamicCollisionVolume = dynamicCollision.CollisionVolume->Transform(dynamicPosition.GetTransform());
			for (auto& staticCollider : ER.GetIterator<Collision, Position>()) {
				auto& staticCollision = staticCollider.Get<Collision>();
				auto& staticPosition = staticCollider.Get<Position>();
				if (
					// This collision has already been handled by the other entity
					!dynamicCollision.Contacts.count(staticCollision.ID)
					// Only handle collisions between disjoint channels
					&& !(dynamicCollision.Channel & staticCollision.Channel)
					// Don't collide with ourself
					&& staticCollision.ID != dynamicCollision.ID 
					// Be within a sane range
					&& (staticPosition.Pos - dynamicPosition.Pos).LengthSquared() < k_collisionCullRange * k_collisionCullRange
					) {
					
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
							
							// Register contacts on both colliders
							contact.Collider = staticCollision.ID;
							dynamicCollision.Contacts[contact.Collider] = contact;

							contact.Collider = dynamicCollision.ID;
							staticCollision.Contacts[contact.Collider] = contact;
						}
					}
				}
			}
		}
	}

	void Room::PlayerLocationUpdate()
	{
		for (auto& playerEntity : ER.GetIterator<Player, Collision>()) {
			auto& player = playerEntity.Get<Player>();
			if (m_type == RoomType::Duck) {
				if (Game::GetInstance().state != GameState::InGame_BossBattle)
				{
					Game::GetInstance().state = GameState::InGame_BossBattle;
					Game::GetInstance().bossStart = clock();
					MultimediaPlayer::SetUp("Boss_Battle_Condesa_DuckAttacks_Overlay.wav", true, true);
				}
			}
		}
	}

	void Room::DoorUpdate(double elapsed)
	{
		for (auto& entity : ER.GetIterator<Door, Model, Collision>()) {
			auto& door = entity.Get<Door>();
			auto& model = entity.Get<Model>();
			auto& collision = entity.Get<Collision>();

			if (door.State == Door::DoorState::Locked) {
				for (auto& playerEntity : ER.GetIterator<Player, Collision>()) {
					auto& player = playerEntity.Get<Player>();
					if (player.Inventory[LootItem::Key] > 0) {
						if (playerEntity.Get<Collision>().Contacts.count(door.ID)) {
							// unlock the door
							door.State = Door::DoorState::Open;
							// consume the key
							player.Inventory[LootItem::Key]--;
							break;
						}
					}
				}
			}

			// Update model
			model.ModelPtr = ModelRepository::Get("door_" + std::to_string((int)door.State));

			/* Update collision channel
			 As long as the player shares this channel, collision will not be handled */
			switch (door.State) {
			case Door::DoorState::Closed:
			case Door::DoorState::Locked:
				collision.Channel = 0;
				break;
			case Door::DoorState::Open:
				collision.Channel = (uint32_t)CollisionChannel::Door;
				break;
			}


		}
	}
	void Room::CombatUpdate(double elapsed)
	{
		bool previousCombatState = m_inCombat;
		// While there are enemy agents in the room, keep the doors closed
		for (auto& agent : ER.GetIterator<Agent>()) {
			if (agent.Get<Agent>().Faction != Agent::AgentFaction::Bread) {
				// Close all the doors if they are open
				m_inCombat = true;
				for (auto& door : ER.GetIterator<Door>()) {
					if (door.Get<Door>().State == Door::DoorState::Open)
						door.Get<Door>().State = Door::DoorState::Closed;
				}
				return;
			}
		}
		// not in combat - Open all the non-locked doors
		m_inCombat = false;
		for (auto& door : ER.GetIterator<Door>()) {
			if (door.Get<Door>().State == Door::DoorState::Closed)
				door.Get<Door>().State = Door::DoorState::Open;
		}
		// Drop loot if the combat state has dropped to false
		if (previousCombatState && !m_inCombat) {
			DropLoot();
		}
	}
	void Room::ItemUpdate(double elapsed)
	{
		for (auto& playerEntity : ER.GetIterator<Player, Collision>()) {
			auto& player = playerEntity.Get<Player>();
			for (auto& entity : ER.GetIterator<Item>()) {
				auto& item = entity.Get<Item>();
				if (playerEntity.Get<Collision>().Contacts.count(item.ID)) {
					// Perform the transaction
					player.Inventory[item.Type]++;
					m_deferredTasks.push_back([=] {
						ER.Remove(item.ID);
					});
				}
			}
		}
		
	}
	void Room::DeferredUpdate(double elapsed)
	{
		for (auto& task : m_deferredTasks) {
			task();
		}
		m_deferredTasks.clear();
	}
}