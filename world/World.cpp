#include "pch.h"

#include <GL/glut.h>

#include "geom/ModelRepository.h"
using geom::ModelRepository;

#include "math/Vector3.h"
using math::Vector3;

#include "World.h"

namespace world {
	World::World()
	{
	}

	void World::Generate()
	{
		ER.CreateEntity(
			Position(Vector3(0.f, 0.f, 0.f))
		);
		ER.CreateEntity(
			Position(Vector3(0.f, 0.f, 0.f)),
			Model(ModelRepository::Get("ogp"))
		);


		ER.CreateEntity(
			Position(Vector3(0.f, 0.f, 0.f)),
			Model(ModelRepository::Get("sphere"))
		);
	}

	void World::Render()
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers


		for (auto& entity : ER.GetIterator<Position, Model>()) {
			Model& modelComp = entity.Get<Model>();
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


		// flush out the buffer contents
		glFinish();
		glutSwapBuffers();
	}

	void World::Update(double elapsed)
	{

	}

	void World::UpdateKeyState(char key, bool state)
	{
		m_keys[key] = state;
	}

	void World::UpdateMousePosition(Vector2 position)
	{

	}
}