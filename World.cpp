#include "pch.h"
#include "World.h"
#include "ModelRepository.h"
World::World()
{
}

void World::Render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

	
	glBegin(GL_TRIANGLES);
	// Blue face
	
	Model& model = *ModelRepository::Get("ogp");
	for (auto& mesh : model.Meshes) {
		for (auto& index : mesh.Indices) {
			ModelMeshVertex& vertex = mesh.Vertices[index];
			glNormal3f(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
			glVertex3f(vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
		}
	}
	
	glEnd();

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
