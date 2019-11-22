#include "pch.h"
#include "World.h"
World::World()
{
}

void World::Render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

	glBegin(GL_TRIANGLES);
	// Blue face
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(1.f, 0.f,0.f);
	glVertex3f(0.f,1.f, 0.f);
	glVertex3f(-1.f, 0.f, 0.f);
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
