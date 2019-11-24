#include "pch.h"

#include <GL/glut.h>

#include "geom/ModelRepository.h"
using geom::ModelRepository;

#include "math/Vector3.h"
using math::Vector3;

#include "World.h"
#include "Player.h"
#include "Agent.h"

namespace world {
	const Vector3 World::k_cameraOffset = Vector3(0.f, 1.f, -1.f);
	World::World()
	{
	}

	void World::Generate()
	{
		shared_ptr<Room> room = std::make_shared<Room>();

		room->GetER().CreateEntity(
			Position(Vector3(0.f, 0.f, 0.f)),
			Model(ModelRepository::Get("ogp"))
		);
		room->GetER().CreateEntity(
			Player(),
			Agent(1.f,1.f),
			Movement(),
			Position(Vector3(0.f, 0.5f, -0.5f)),
			Model(ModelRepository::Get("sphere"))
		);
		m_localRooms.push_back(room);
		m_currentRoom = room;
	}

	void World::RenderUpdate(double elapsed)
	{
		
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glOrtho(-1, 1, -1, 1, 0, 2);
		
		int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
		int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		
		gluPerspective(70.0, (double)windowWidth / windowHeight, 0.01, 10.0);
		float pos[]{ 0.f,1.f,0.f,1.f };
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		// Get the player's position
		for (auto& entity : m_currentRoom->GetER().GetIterator<Player, Position>()) {
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
		
		for (auto& room : m_localRooms) {
			glTranslatef(room->GetPosition().X, room->GetPosition().Y, room->GetPosition().Z);
			glPushMatrix();
			for (auto& entity : room->GetER().GetIterator<Model, Position>()) {
				Position& position = entity.Get<Position>();
				Model& modelComp = entity.Get<Model>();
				glTranslatef(position.Pos.X, position.Pos.Y, position.Pos.Z);
				glBegin(GL_TRIANGLES);

				geom::Model& model = *modelComp.ModelPtr;
				for (auto& mesh : model.Meshes) {
					for (auto& index : mesh.Indices) {
						geom::ModelMeshVertex& vertex = mesh.Vertices[index];
						glNormal3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
						glVertex3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
					}
				}

				glEnd();
			}
			glPopMatrix();
		}
		glPopMatrix();
		


		// flush out the buffer contents
		glFinish();
		glutSwapBuffers();
	}

	void World::PlayerUpdate(double elapsed)
	{
		for (auto& entity : m_currentRoom->GetER().GetIterator<Player, Agent>()) {
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

		// Perform room system updates
		for (auto& room : m_localRooms) {
			room->Update(elapsed);
		}
		// Render the scene after all game systems have been updated
		RenderUpdate(elapsed);
	}

	void World::UpdateKeyState(char key, bool state)
	{
		m_keys[key] = state;
	}

	void World::UpdateMousePosition(Vector2 position)
	{

	}
}