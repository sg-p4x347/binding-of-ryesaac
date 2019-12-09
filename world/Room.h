#pragma once
#include "ecs/EntityRepository.h"

// Components
#include "Position.h"
#include "Movement.h"
#include "Collision.h"
#include "Model.h"
#include "Agent.h"
#include "Player.h"
#include "Door.h"
#include "AI.h"
#include "Item.h"
#include "Sweep.h"

#include "LootItem.h"

namespace world {
	typedef ecs::EntityRepository<
		Position,
		Movement,
		Collision,
		Model,
		Agent,
		Player,
		Door,
		AI,
		Item,
		Sweep
	> EntityRepository;

	enum class CollisionChannel {
		Door = 1,
		Projectile = 2
	};
	class Room
	{
	public:
		enum class RoomType {
			Root,
			Normal,
			Duck
		};
		Room(RoomType type, Vector3 center);
		void Update(double elapsed);
		void Render();
		EntityRepository& GetER();
		RoomType GetType();
		void AddLoot(LootItem item);
		void DropLoot();
		void SweepAttack(int cornerIndex);
		ecs::EntityID CreateDuck();
		void StompAttack(Vector2 focus);
	private:
		void AgentUpdate(double elapsed);
		void AiUpdate(double elapsed);
		void MovementUpdate(double elapsed);
		void CollisionUpdate(double elapsed);
		void PlayerLocationUpdate();
		// Updates door model and state based on the current door state and interactions
		void DoorUpdate(double elapsed);
		// Ensures that all doors are closed while in combat
		void CombatUpdate(double elapsed);
		// Transfer world items to the player inventory when in contact
		void ItemUpdate(double elapsed);
		// Update sweeps
		void SweepUpdate(double elapsed);

		void DeferredUpdate(double elapsed);
		
	private:
		EntityRepository ER;
		static const float k_collisionCullRange;
		vector<function<void()>> m_deferredTasks;
		vector<LootItem> m_loot;
		Vector3 m_center;
		RoomType m_type;
		ecs::EntityID m_duck;
		bool m_inCombat;
	};
}
