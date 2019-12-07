#pragma once
#include "pch.h"

#include <windows.h>

#include "math/Vector2.h"
using math::Vector2;

#include "math/IntVec2.h"
using math::IntVec2;

#include "ecs/EntityRepository.h"
using ecs::EntityRepository;

#include "Room.h"
#include "Door.h"
#include "GraphNode.h"
#include "Door.h"

namespace world {
	struct RoomGenerationUnit {
		shared_ptr<Door> Doors[4];
	};
	struct IntVec2Comparer {
		bool operator()(const IntVec2& left, const IntVec2& right) const {
			static const int k_maxIntRoot = 46340;
			return (left.X + k_maxIntRoot / 2) + left.Y * k_maxIntRoot < (right.X + k_maxIntRoot / 2) + right.Y * k_maxIntRoot;
		}
	};
	class World
	{
	public:
		World();
		void Generate();
		// Dimensions are in tile units
		shared_ptr<GraphNode<Room>> GenerateRoomNode();

		void Update();
		void Render();
		void PlayerUpdate(double elapsed);

		void UpdateKeyState(char key, bool state);
		void UpdateSpecialKeyState(int key, bool state);
		void UpdateMousePosition(Vector2 position);
	private:
		//----------------------------------------------------------------
		// Local graph operations
		shared_ptr<GraphNode<Room>> UpdateCurrentNode(Vector3 focus);
		IntVec2 GetUnitPosition(Vector3 worldPosition);
		/* Searches adjacent nodes for a door with the specified position,
		and updates it's state accordingly */
		void UpdateDoorState(Vector3 position, Door door);
		//----------------------------------------------------------------
		// Generation
		bool Occupied(IntVec2 position,  map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& map);
		bool Occupied(vector<IntVec2> positions, map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& map);
		
		vector<IntVec2> CreateRoomUnitSet(IntVec2 entrance, IntVec2 direction);
		void BakeRoomUnits(map<IntVec2,RoomGenerationUnit,IntVec2Comparer> & roomUnits, Room & room);
		void SpawnToasters(map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& roomUnits, Room& room);
		int RollDoorCount(int max);
		int RollRoomUnits();
		

	private:
		LARGE_INTEGER m_lastUpdate;
		map<char, bool> m_keys;
		map<int, bool> m_specialKeys;
		shared_ptr<GraphNode<Room>> m_currentNode;
		shared_ptr<GraphNode<Room>> m_nextCurrentNode;
		vector<ecs::EntityID> m_removedEntities;
		map<IntVec2, shared_ptr<GraphNode<Room>>, IntVec2Comparer> m_roomMap;

		//----------------------------------------------------------------
		// Constants
		static const float k_tileSize;
		static const IntVec2 k_roomUnitSize;
		static const int k_roomCount;
		static const int k_maxRoomUnits; // each room unit is k_roomWidth tiles wide
		static const int k_minRoomUnits; // each room unit is k_roomWidth tiles wide
		static const int k_maxBranchingSize; // the maximum number of outward facing doors a room can have
		static const Vector3 k_cameraOffset;

		static const float k_toasterProbability; // The probability that a toaster will spawn on a tile
	};
}
