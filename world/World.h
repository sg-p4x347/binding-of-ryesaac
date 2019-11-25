#pragma once
#include "pch.h"

#include <windows.h>

#include "math/Vector2.h"
using math::Vector2;

#include "ecs/EntityRepository.h"
using ecs::EntityRepository;

#include "Room.h"
#include "GraphNode.h"

namespace world {
	class World
	{
	public:
		World();
		void Generate();
		// Dimensions are in tile units
		shared_ptr<GraphNode<Room>> GenerateRoomNode(Vector3 worldPosition, int width, int length, int height);

		void Update();
		void Render();
		void PlayerUpdate(double elapsed);

		void UpdateKeyState(char key, bool state);
		void UpdateMousePosition(Vector2 position);
	private:
		// Test a rectangular prism for intersection with all current rooms
		bool RoomIntersectionTest(Vector3 position, Vector3 size, shared_ptr<GraphNode<Room>> start);
		shared_ptr<GraphNode<Room>> GetContainingNode(Vector3 point, shared_ptr<GraphNode<Room>> start, shared_ptr<GraphNode<Room>> exclude);

		int RollRoomUnits();
		int RoomUnitsToWidthTiles(int roomUnits);
		int RoomUnitsToLengthTiles(int roomUnits);
	private:
		LARGE_INTEGER m_lastUpdate;
		map<char, bool> m_keys;
		shared_ptr<GraphNode<Room>> m_currentNode;

		//----------------------------------------------------------------
		// Constants
		static const float k_tileSize;
		static const int k_roomWidth; // in tiles
		static const int k_roomLength; // in tiles
		static const int k_roomHeight; // in tiles
		static const int k_roomCount;
		static const int k_maxRoomUnits; // each room unit is k_roomWidth tiles wide
		static const int k_minRoomUnits; // each room unit is k_roomWidth tiles wide

		static const Vector3 k_cameraOffset;
	};
}
