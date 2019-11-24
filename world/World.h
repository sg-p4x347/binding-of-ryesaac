#pragma once
#include "pch.h"

#include <windows.h>

#include "math/Vector2.h"
using math::Vector2;

#include "ecs/EntityRepository.h"
using ecs::EntityRepository;

#include "Room.h"

namespace world {
	class World
	{
	public:
		World();
		void Generate();

		void Update();
		void RenderUpdate(double elapsed);
		void PlayerUpdate(double elapsed);

		void UpdateKeyState(char key, bool state);
		void UpdateMousePosition(Vector2 position);
	private:
		LARGE_INTEGER m_lastUpdate;
		map<char, bool> m_keys;
		shared_ptr<Room> m_currentRoom;
		vector<shared_ptr<Room>> m_localRooms;
		vector<shared_ptr<Room>> m_rooms;

		//----------------------------------------------------------------
		// Constants
		static const float k_tileSize;
		static const int k_roomWidth; // in tiles
		static const int k_roomLength; // in tiles
		static const Vector3 k_cameraOffset;
	};
}
