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
#include "AI.h"

namespace world {
	const float World::k_tileSize = 1.f;
	const IntVec2 World::k_roomUnitSize = IntVec2(14,8);
	const int World::k_maxRoomUnits = 1;
	const int World::k_minRoomUnits = 1;
	const int World::k_maxBranchingSize = 8;
	
	const int World::k_roomCount = 2;
	const Vector3 World::k_cameraOffset = Vector3(0.f, 8.f, -4.f);

	const float World::k_toasterProbability = 0.01f;

	
	//const Vector3 World::k_cameraOffset = Vector3(0.f, 1.f, -1.f);
	World::World()
	{
	}

	void World::Generate()
	{
		// Seed the RNG with the current time for different results on every run
		//srand(time(NULL));
		int roomCount = 0;


		// create the starting room
		auto seed = GenerateRoomNode();
		roomCount++;
		seed->Data.GetER().CreateEntity(
			Player(),
			Agent(Agent::AgentFaction::Bread,8.f,1,0.5f,2.f,1),
			Movement(),
			Position(Vector3(0.f,0.f,0.f), Vector3()),
			Model(ModelRepository::Get("sphere")),
			Collision(std::make_shared<Sphere>(Vector3::Zero, 0.25f))
		);

		map<IntVec2, RoomGenerationUnit, IntVec2Comparer> roomUnits;

		
		queue<vector<IntVec2>> rooms;
		queue<shared_ptr<GraphNode<Room>>> roomNodes;
		vector<IntVec2> seedRoom = CreateRoomUnitSet(IntVec2::Zero, IntVec2(0, 1));
		rooms.push(seedRoom);
		roomNodes.push(seed);

		for (auto& newUnit : seedRoom) {
			roomUnits[newUnit] = RoomGenerationUnit();
		}
		while (!rooms.empty()) {

			vector<IntVec2> hub = rooms.front();
			auto roomNode = roomNodes.front();
			int maxDoorCount = RollDoorCount(min(k_maxBranchingSize, k_roomCount - roomCount));
			int doorCount = 0;
			for (auto& unit : hub) {
				if (doorCount >= maxDoorCount)
					break;
				for (int i = 0; i < 4 && doorCount < maxDoorCount; i++) {
					double x = std::cos((PI / 2.0) * i);
					double y = std::sin((PI / 2.0) * i);
					IntVec2 direction(round(x), round(y));
					IntVec2 adjacent = unit + direction;
					if (!Occupied(adjacent, roomUnits)) {
						vector<IntVec2> adjacentRoom = CreateRoomUnitSet(adjacent, direction);
						if (!Occupied(adjacentRoom, roomUnits)) {
							doorCount++;
							roomCount++;
							// add the room to the queue to become a new hub
							math::Shuffle(adjacentRoom);
							rooms.push(adjacentRoom);
							
							// Create a room node
							auto adjacentRoomNode = GenerateRoomNode();
							adjacentRoomNode->AdjacentNodes.push_back(roomNode);
							roomNode->AdjacentNodes.push_back(adjacentRoomNode);
							roomNodes.push(adjacentRoomNode);

							// add the room units
							for (auto& newUnit : adjacentRoom) {
								roomUnits[newUnit] = RoomGenerationUnit();
							}
							roomUnits[adjacent].Doors[(i + 2) % 4] = std::make_shared<Door>();
							roomUnits[unit].Doors[i] = std::make_shared<Door>();

						}
					}
				}
			}
			// this hub is done
			rooms.pop();
			roomNodes.pop();
			map<IntVec2, RoomGenerationUnit, IntVec2Comparer> roomMap;
			for (auto& unitPos : hub) {
				roomMap[unitPos] = roomUnits[unitPos];
				// add this nodes to the full map
				m_roomMap[unitPos] = roomNode;
			}

			// bake each unit into entities
			BakeRoomUnits(roomMap, roomNode->Data);
			SpawnToasters(roomMap, roomNode->Data);
		}
		
		
		m_currentNode = seed;
	}

	shared_ptr<GraphNode<Room>> World::GenerateRoomNode()
	{
		return std::make_shared<GraphNode<Room>>();
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
		Vector3 playerPosition;
		ecs::EntityID playerID = 0;
		for (auto& entity : m_currentNode->Data.GetER().GetIterator<Player, Agent, Position>()) {
			auto & player = entity.Get<Player>();
			auto& agent = entity.Get<Agent>();
			auto& position = entity.Get<Position>();

			playerID = player.ID;
			playerPosition = position.Pos;

			agent.Heading = Vector3();
			if (m_keys['w'])
				agent.Heading.Z += 1.f;
			if (m_keys['a'])
				agent.Heading.X += 1.f;
			if (m_keys['s'])
				agent.Heading.Z -= 1.f;
			if (m_keys['d'])
				agent.Heading.X -= 1.f;

			agent.Heading.Normalize();

			Vector3 facing;
			if (m_specialKeys[GLUT_KEY_UP])
				facing.Z += 1.f;
			if (m_specialKeys[GLUT_KEY_LEFT])
				facing.X += 1.f;
			if (m_specialKeys[GLUT_KEY_DOWN])
				facing.Z -= 1.f;
			if (m_specialKeys[GLUT_KEY_RIGHT])
				facing.X -= 1.f;

			if (facing.LengthSquared() > 0.f) {
				facing.Normalize();
				// Rotate the player
				position.Rot.Y = std::atan2(facing.Z, facing.X);
				// An arrow key is pressed, start attacking
				agent.Attack = true;
			}
			else {
				// The arrow keys are not being pressed, stop attacking
				agent.Attack = false;
			}
		}
		m_nextCurrentNode = UpdateCurrentNode(playerPosition);
		if (playerID && m_nextCurrentNode && m_currentNode != m_nextCurrentNode) {
			// Remove the player from this node and place them in the new current node
			EntityRepository::Copy(playerID, m_currentNode->Data.GetER(), m_nextCurrentNode->Data.GetER());
			
			m_removedEntities.push_back(playerID);
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
		
		// Update the current node and perform post-update actions
		for (auto& id : m_removedEntities) {
			m_currentNode->Data.GetER().Remove(id);
		}
		m_removedEntities.clear();
		if (m_nextCurrentNode)
			m_currentNode = m_nextCurrentNode;
	}

	void World::UpdateKeyState(char key, bool state)
	{
		m_keys[key] = state;
	}

	void World::UpdateSpecialKeyState(int key, bool state)
	{
		m_specialKeys[key] = state;
	}

	void World::UpdateMousePosition(Vector2 position)
	{

	}
	shared_ptr<GraphNode<Room>> World::UpdateCurrentNode(Vector3 focus)
	{
		IntVec2 unitFocus = GetUnitPosition(focus);
		auto it = m_roomMap.find(unitFocus);
		if (it != m_roomMap.end()) {
			if (it->second != m_currentNode) {
				return it->second;
			}
		}
		return nullptr;
	}
	IntVec2 World::GetUnitPosition(Vector3 worldPosition)
	{
		return IntVec2(round(worldPosition.X / (k_roomUnitSize.X * k_tileSize)), round(worldPosition.Z / (k_roomUnitSize.Y * k_tileSize)));
	}

	void World::UpdateDoorState(Vector3 position, Door door)
	{
		for (auto& adjacentNode : m_currentNode->AdjacentNodes) {
			for (auto& entity : adjacentNode->Data.GetER().GetIterator<Door, Position>()) {
				auto& adjacentDoor = entity.Get<Door>();
				auto& adjacentPosition = entity.Get<Position>();
				if (adjacentPosition.Pos == position) {
					adjacentDoor.State = door.State;
				}
			}
		}
	}
	
	bool World::Occupied(IntVec2 position, map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& map)
	{
		return map.count(position);
	}
	bool World::Occupied(vector<IntVec2> positions, map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& map)
	{
		for (auto& position : positions) {
			if (Occupied(position, map))
				return true;
		}
		return false;
	}
	vector<IntVec2> World::CreateRoomUnitSet(IntVec2 entrance, IntVec2 direction)
	{
		IntVec2 tangent = IntVec2(-direction.Y, direction.X);
		int width = RollRoomUnits();
		int length = RollRoomUnits();

		IntVec2 start = entrance + IntVec2(
			min(tangent.X * width / 2, -tangent.X * width / 2),
			min(tangent.Y * length / 2, -tangent.Y * length / 2)
			);
		vector<IntVec2> room;
		for (int x = start.X; x < start.X + width; x++) {
			for (int y = start.Y; y < start.Y + length; y++) {
				room.push_back(IntVec2(x, y));
			}
		}
		return room;
	}
	void World::BakeRoomUnits(map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& roomUnits, Room& room)
	{
		auto doorwayModel = ModelRepository::Get("doorway");
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
		inside   ^
			   __|
		      |  |
		X <------*

		*/
		auto floorModel = ModelRepository::Get("floor");
		for (auto& unit : roomUnits) {
			Vector3 unitCenter = Vector3(unit.first.X * k_roomUnitSize.X * k_tileSize,0.f,unit.first.Y * k_roomUnitSize.Y * k_tileSize);
			Vector3 unitSize = Vector3(k_roomUnitSize.X * k_tileSize, 1.f, k_roomUnitSize.Y * k_tileSize);
			// floor
			room.GetER().CreateEntity(
				Position(unitCenter, Vector3::Zero),
				Model(floorModel)
			);

			for (int i = 0; i < 4; i++) {
				double rotation = (PI / 2.0) * i;
				double x = std::cos(rotation);
				double y = std::sin(rotation);

				IntVec2 unitDir(round(x), round(y));
				IntVec2 unitTangent(unitDir.Y, -unitDir.X);
				IntVec2 adjacent = unit.first + unitDir;

				if (!roomUnits.count(adjacent)) {
					// This is a wall
					
					Vector3 wallNormal = Vector3(unitDir.X, 0.f, unitDir.Y);
					Vector3 wallDir = -Vector3(unitTangent.X, 0.f, unitTangent.Y);

					int wallLength = abs(unitSize.Dot(wallDir));
					Vector3 wallStart = unitCenter + wallNormal * abs(unitSize.Dot(wallNormal) / 2.0) - wallDir * wallLength / 2;
					for (int tileIndex = 0; tileIndex < wallLength; tileIndex++) {
						if (tileIndex == 0 && !roomUnits.count(unit.first + unitTangent)) {
							// Corner
							room.GetER().CreateEntity(
								Position(wallStart + wallDir * tileIndex, Vector3(0.f, -rotation - PI / 2.0, 0.f)),
								Model(cornerModel),
								Collision(geom::CreateBox(Vector3::Zero, Vector3(1.f, 1.f, 1.f)))
							);
						}
						else {
							bool doorway = unit.second.Doors[i] && tileIndex == wallLength / 2;
							if (!doorway) {
								room.GetER().CreateEntity(
									Position(wallStart + wallDir * tileIndex, Vector3(0.f, -rotation - PI / 2.0, 0.f)),
									Model(wallModel),
									Collision(geom::CreateBox(Vector3::Zero, Vector3(1.f, 1.f, 1.f)))
								);
							}
							else {
								Door door = *unit.second.Doors[i];
								Position position = Position(wallStart + wallDir * tileIndex, Vector3(0.f, -rotation - PI / 2.0, 0.f));
								// Doorway
								room.GetER().CreateEntity(
									position,
									Model(ModelRepository::Get("doorway_" + std::to_string((int)door.Type)))
								);
								// Door
								Model model = Model(ModelRepository::Get("door_" + std::to_string((int)door.State)));
								if (door.State == Door::DoorState::Open) {
									room.GetER().CreateEntity(
										door,
										position,
										model
									);
								}
								else {
									room.GetER().CreateEntity(
										door,
										position,
										model,
										Collision(geom::CreateBox(Vector3::Zero, Vector3(1.f, 1.f, 1.f)))
									);
								}
							}
						}
					}
				}
			}
		}
	}
	void World::SpawnToasters(map<IntVec2, RoomGenerationUnit, IntVec2Comparer>& roomUnits, Room& room)
	{
		auto toasterModel = ModelRepository::Get("toaster");
		for (auto& unit : roomUnits) {
			Vector3 unitCenter = Vector3(unit.first.X * k_roomUnitSize.X * k_tileSize, 0.f, unit.first.Y * k_roomUnitSize.Y * k_tileSize);
			Vector3 unitSize = Vector3(k_roomUnitSize.X * k_tileSize, 1.f, k_roomUnitSize.Y * k_tileSize);
			for (int x = unitCenter.X - unitSize.X / 2.0 + 1; x < unitCenter.X + unitSize.X / 2.0; x++) {
				for (int z = unitCenter.Z - unitSize.Z / 2.0 + 1; z < unitCenter.Z + unitSize.Z / 2.0;z++) {
					if (math::Chance(k_toasterProbability)) {
						room.GetER().CreateEntity(
							Position(Vector3(x,unitCenter.Y,z), Vector3::Zero),
							Model(toasterModel),
							Movement(),
							Agent(Agent::AgentFaction::Toast, 2.f, 4, 6, 0.f, 1),
							AI()
						);
					}
				}
			}
			
		}
	}
	int World::RollDoorCount(int max)
	{
		return math::RandWithin(min(1,max), max);
	}
	int World::RollRoomUnits()
	{
		return math::RandWithin(k_minRoomUnits, k_maxRoomUnits);
	}
}