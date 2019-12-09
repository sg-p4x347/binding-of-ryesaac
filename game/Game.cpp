#include "pch.h"

#include "Game.h"

namespace game
{
	Game::Game()
	{

	}

	Game& Game::GetInstance()
	{
		static Game instance;
		return instance;
	}

	void Game::mouseMoveHandler(int cursorX, int cursorY)
	{
		if (GetInstance().g_world)
			GetInstance().g_world->UpdateMousePosition(Vector2(cursorX, cursorY));
	}

	void Game::keyHandler(unsigned char key, int x, int y)
	{
		if (GetInstance().g_world)
			GetInstance().g_world->UpdateKeyState(key, true);
	}

	void Game::keyUpHandler(unsigned char key, int x, int y)
	{
		if (GetInstance().g_world)
			GetInstance().g_world->UpdateKeyState(key, false);
	}

	void Game::specialHandler(int key, int x, int y)
	{
		if (GetInstance().g_world)
			GetInstance().g_world->UpdateSpecialKeyState(key, true);
	}

	void Game::specialUpHandler(int key, int x, int y)
	{
		if (GetInstance().g_world)
			GetInstance().g_world->UpdateSpecialKeyState(key, false);
	}

	void Game::GenerateWorld()
	{
		EnableLighting();
		GetInstance().g_world = std::make_unique<World>();
		if (GetInstance().g_world)
			GetInstance().g_world->Generate();
	}

	void Game::EnableLighting()
	{
		glEnable(GL_LIGHT0);
		float pos[]{ 1.f,1.f,0.f,1.f };
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		float spotDir[]{ 0.f,-1.f,0.f };
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
		float ambient[]{ 0.5f,0.5f,0.5f,1.f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	}

	void Game::Update()
	{
		if (g_world) g_world->Update();
		else
		{
			double elapsed;
			if (state == GameState::None)
			{
				state = GameState::Intro;
				// Set up audio
				MultimediaPlayer::SetUp("./Assets/audio/Intro_Condesa_Vox_Overlay.wav", true, false);
				activeSlide = 0;
				
				// Set up a timer for sync uses and start music
				gameStart = clock();
				MultimediaPlayer::GetInstance().startAudio();
				slideStart = clock();
			}
			else if (state == GameState::Intro)
			{
				clock_t current = clock();
				elapsed = (current - gameStart) / (double)CLOCKS_PER_SEC;
				double sum = 0;
				for (int i = 0; i < slideShow.size(); ++i)
				{
					sum += slideShow[i].getDuration();
					if (elapsed <= sum)
					{
						activeSlide = i;
						i = slideShow.size();
					}
				}
			}
			else if (state == GameState::MainMenu)
			{
				
			}
		}
	}

	void Game::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

		if (g_world) { g_world->Render(); }
		else
		{
			Vector2 position = { 0.f, 0.f };
			Vector2 size = { 1.f, (float)glutGet(GLUT_WINDOW_HEIGHT) / (float)glutGet(GLUT_WINDOW_WIDTH) };
			string texture = slideShow[activeSlide].getTexName();
			//glRasterPos2d(0, 0);
			glLoadIdentity();
			gluOrtho2D(0, 1, 0, (float)glutGet(GLUT_WINDOW_HEIGHT) / (float)glutGet(GLUT_WINDOW_WIDTH));
			RenderQuad(position, size, texture);
		}

		// flush out the buffer contents
		glFinish();
		glutSwapBuffers();
	}

	void Game::RenderQuad(Vector2 position, Vector2 size, string texture)
	{
		glBindTexture(GL_TEXTURE_2D, TextureRepository::GetID(texture));
		glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f);
		glVertex2f(position.X, position.Y);
		glTexCoord2f(1.f, 0.f);
		glVertex2f(position.X + size.X, position.Y);
		glTexCoord2f(1.f, 1.f);
		glVertex2f(position.X + size.X, position.Y + size.Y);
		glTexCoord2f(0.f, 1.f);
		glVertex2f(position.X, position.Y + size.Y);
		glEnd();
	}
}