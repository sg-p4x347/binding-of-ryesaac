#include "pch.h"

#include <GL/glut.h>

#include "geom/ModelRepository.h"
using geom::ModelRepository;

#include "geom/Sphere.h"
using geom::Sphere;

#include "geom/CollisionUtil.h"

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "math/Vector3.h"
using math::Vector3;

#include "math/Utility.h"
using math::PI;

#include "World.h"
#include "Player.h"
#include "Agent.h"
#include "Door.h"

namespace world {
	const float World::k_tileSize = 1.f;
	const int World::k_roomWidth = 13;
	const int World::k_roomLength = 7;
	const int World::k_roomHeight = 1;
	const int World::k_maxRoomUnits = 2;
	const int World::k_minRoomUnits = 1;
	
	const Vector3 World::k_cameraOffset = Vector3(0.f, 8.f, -4.f);

	//const Vector3 World::k_cameraOffset = Vector3(0.f, 1.f, -1.f);
	World::World()
	{
	}

	void World::Generate()
	{
		int roomCount = 0;

		// create the starting room
		auto seed = GenerateRoomNode(Vector3::Zero, RoomUnitsToWidthTiles(2), RoomUnitsToWidthTiles(1), k_roomHeight);
		roomCount++;
		seed->Data.GetER().CreateEntity(
			Player(),
			Agent(8.f, 1.f),
			Movement(),
			Position(Vector3(2.f, 0.f, 2.f), Vector3()),
			Model(ModelRepository::Get("sphere")),
			Collision(std::make_shared<Sphere>(Vector3::Zero, 0.25f))
		);

		/*perimeter.push(seed);
		
		while (roomCount < k_roomCount) {

			


			int width = RoomUnitsToWidthTiles(RollRoomUnits());
			int height = RoomUnitsToWidthTiles(RollRoomUnits());
			GenerateRoomNode(Vector3::Zero, RoomUnitsToWidthTiles(2), RoomUnitsToWidthTiles(1), k_roomHeight);
		}
		
		*/
		m_currentNode = seed;
	}

	shared_ptr<GraphNode<Room>> World::GenerateRoomNode(Vector3 worldPosition, int width, int length, int height)
	{
		// Add one to dimensions to be inclusive
		width++;
		length++;
		height++;
		shared_ptr<GraphNode<Room>> node = std::make_shared<GraphNode<Room>>(Room(worldPosition, Vector3(width * k_tileSize, height * k_tileSize, length * k_tileSize)));
		Room& room = node->Data;
		auto wallModel = ModelRepository::Get("wall");
		/*
				 Z
		  inside ^
			_____|
		   |     |
		X <------*

		*/
		auto cornerModel = ModelRepository::Get("corner");
		/*
				 Z
		inside __^
			__|  |
		   |     |
		X <------*

		*/
		auto floorModel = ModelRepository::Get("floor");

		Vector3 xWall = Vector3::UnitX * width * k_tileSize;
		Vector3 zWall = Vector3::UnitZ * length * k_tileSize;
		vector<Vector3> waypoints{
			worldPosition,
			worldPosition + xWall,
			worldPosition + xWall + zWall,
			worldPosition + zWall
		};
		vector<Position> possibleDoors;
		for (int i = 0; i < waypoints.size(); i++) {
			Vector3& waypoint = waypoints[i];
			Vector3& nextWaypoint = waypoints[(i + 1) % waypoints.size()];

			Vector3 wall = nextWaypoint - waypoint;
			Vector3 wallDir = wall.Normalized();

			Vector3 rotation = Vector3(0.f, -std::atan2f(wallDir.Z, wallDir.X), 0.f);

			// Add corner at this waypoint
			room.GetER().CreateEntity(
				Position(waypoint, rotation),
				Model(cornerModel)
			);
			// Add wall tiles from this waypoint to the next
			int wallLength = wall.Length();
			for (int tileIndex = 1; tileIndex < wallLength; tileIndex++) {

				bool doorway =
					xWall.Dot(wallDir) != 0.f && (tileIndex % k_roomWidth - 1 == k_roomWidth / 2)
					||
					zWall.Dot(wallDir) != 0.f && (tileIndex % k_roomLength - 1 == k_roomLength / 2);
				if (!doorway) {
					room.GetER().CreateEntity(
						Position(waypoint + wallDir * tileIndex, rotation),
						Model(wallModel),
						Collision(geom::CreateBox(Vector3::Zero, Vector3(1.f, 1.f, 1.f)))
					);
				}
				else {
					possibleDoors.push_back(Position(waypoint + wallDir * tileIndex, rotation));
				}
			}
		}
		// Add floors
		for (int x = 0; x <= width; x++) {
			for (int z = 0; z <= length; z++) {
				room.GetER().CreateEntity(
					Position(worldPosition - Vector3::UnitY + Vector3::UnitX * x * k_tileSize + Vector3::UnitZ * z * k_tileSize, Vector3::Zero),
					Model(floorModel)
				);
			}
		}
		
		int desiredDoorCount = math::RandWithin(1, 3);
		int doorCount = 0;
		vector<Position> missingDoors;
		
		// Remove door slots that are blocked by an existing room
		for (int i = 0; i < possibleDoors.size(); i++)
			if (GetContainingNode(possibleDoors[i].Pos, node,node)) {
				missingDoors.push_back(possibleDoors[i]);
				possibleDoors.erase(possibleDoors.begin() + i);
				i--;
			}

		// Keep removing a random possible door slot until we have reached the desired count
		while (!possibleDoors.empty() && possibleDoors.size() > desiredDoorCount) {
			int doorIndex = math::RandWithin(0, possibleDoors.size() - 1);
			missingDoors.push_back(possibleDoors[doorIndex]);
			possibleDoors.erase(possibleDoors.begin() + doorIndex);
		}

		// Create doors
		for (auto& possibleDoor : possibleDoors) {

		}
		// Fill in missing door holes
		for (auto& missingDoor : missingDoors) {
			room.GetER().CreateEntity(
				missingDoor,
				Model(wallModel),
				Collision(geom::CreateBox(Vector3::Zero, Vector3(1.f, 1.f, 1.f)))
			);
		}

		/*for (auto& entity : room.GetER().GetIterator<Door, Position>()) {
			auto& position = entity.Get<Position>();
			if (GetContainingNode(position.Pos, node)) {
				missingDoors
			}
		}*/
		

		return node;
	}

	void World::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glOrtho(-1, 1, -1, 1, 0, 2);
		
		int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
		int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		
		gluPerspective(70.0, (double)windowWidth / windowHeight, 0.01, 1000.0);
		
		// Get the player's position
		for (auto& entity : m_currentNode->Data.GetER().GetIterator<Player, Position>()) {
			auto& player = entity.Get<Player>();
			auto& position = entity.Get<Position>();
			Vector3 camera = position.Pos + k_cameraOffset;
			Vector3 lookDir = position.Pos - camera;
			gluLookAt(
				// Eye position
				camera.X, camera.Y, camera.Z,
				// Look at position
				camera.X + lookDir.X, camera.Y + lookDir.Y, camera.Z + lookDir.Z,
				// Up
				0.f, 1.f, 0.f
			);
			
		}
		
		glPushMatrix();
		
		m_currentNode->Data.Render();
		for (auto& adjacentNode : m_currentNode->AdjacentNodes)
			adjacentNode->Data.Render();

		glPopMatrix();
		


		// flush out the buffer contents
		glFinish();
		glutSwapBuffers();
	}

	void World::PlayerUpdate(double elapsed)
	{
		for (auto& entity : m_currentNode->Data.GetER().GetIterator<Player, Agent>()) {
			auto & player = entity.Get<Player>();
			auto& agent = entity.Get<Agent>();

			agent.Heading = Vector3();
			if (m_keys['w'])
				agent.Heading.Z += 1.f;
			if (m_keys['a'])
				agent.Heading.X += 1.f;
			if (m_keys['s'])
				agent.Heading.Z -= 1.f;
			if (m_keys['d'])
				agent.Heading.X -= 1.f;
		}
	}

	void World::Update()
	{
		LARGE_INTEGER current;
		if (!QueryPerformanceFrequency(&current))
			cout << "QueryPerformanceFrequency failed!\n";

		double frequency = double(current.QuadPart);

		QueryPerformanceCounter(&current);
		double elapsed = (current.QuadPart - m_lastUpdate.QuadPart) / frequency;
		m_lastUpdate = current;

		// Perform world system updates
		PlayerUpdate(elapsed);

		// Perform room system updates in the current room and adjacent rooms
		m_currentNode->Data.Update(elapsed);
		for (auto& adjacentNode : m_currentNode->AdjacentNodes)
			adjacentNode->Data.Update(elapsed);
		// Render the scene after all game systems have been updated
		(elapsed);
	}

	void World::UpdateKeyState(char key, bool state)
	{
		m_keys[key] = state;
	}

	void World::UpdateMousePosition(Vector2 position)
	{

	}
	bool World::RoomIntersectionTest(Vector3 position, Vector3 size, shared_ptr<GraphNode<Room>> start)
	{
		if (start) {
			set<shared_ptr<GraphNode<Room>>> checked;
			queue<shared_ptr<GraphNode<Room>>> unchecked;
			unchecked.push(start);
			while (!unchecked.empty()) {
				shared_ptr<GraphNode<Room>> node = unchecked.front();
				Room& room = node->Data;
				unchecked.pop();
				if (math::IntersectionTest(position.X, position.X + size.X, room.GetPosition().X, room.GetPosition().X + room.GetSize().X)
					&& math::IntersectionTest(position.Y, position.Y + size.Y, room.GetPosition().Y, room.GetPosition().Y + room.GetSize().Y)
					&& math::IntersectionTest(position.Z, position.Z + size.Z, room.GetPosition().Z, room.GetPosition().Z + room.GetSize().Z)) {
					return true;
				}
				checked.insert(node);
				// add unchecked adjacent nodes to the queue
				for (auto& adjacentNode : node->AdjacentNodes)
					if (!checked.count(adjacentNode))
						unchecked.push(adjacentNode);
			}
		}
	}
	shared_ptr<GraphNode<Room>> World::GetContainingNode(Vector3 point, shared_ptr<GraphNode<Room>> start, shared_ptr<GraphNode<Room>> exclude)
	{
		if (start) {
			set<shared_ptr<GraphNode<Room>>> checked;
			queue<shared_ptr<GraphNode<Room>>> unchecked;
			unchecked.push(start);
			while (!unchecked.empty()) {
				shared_ptr<GraphNode<Room>> node = unchecked.front();
				Room& room = node->Data;
				unchecked.pop();
				if (node != exclude && room.Contains(point)) {
					return node;
				}
				checked.insert(node);
				// add unchecked adjacent nodes to the queue
				for (auto& adjacentNode : node->AdjacentNodes)
					if (!checked.count(adjacentNode))
						unchecked.push(adjacentNode);
			}
		}
		return nullptr;
	}
	int World::RollRoomUnits()
	{
		return math::RandWithin(k_minRoomUnits, k_maxRoomUnits);
	}
	int World::RoomUnitsToWidthTiles(int roomUnits)
	{
		return roomUnits * (k_roomWidth - 1) + 1;
	}
	int World::RoomUnitsToLengthTiles(int roomUnits)
	{
		return roomUnits * (k_roomLength - 1) + 1;
	}
}