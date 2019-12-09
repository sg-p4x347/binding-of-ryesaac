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
		if (GetInstance().g_world && GetInstance().state != GameState::Outro && GetInstance().state != GameState::Death)
			GetInstance().g_world->UpdateKeyState(key, true);
		else
		{
			if (key == 13)
			{
				switch ((int)Game::GetInstance().state)
				{
				case (int)GameState::Intro:
					Game::GetInstance().state = GameState::MainMenu;
					Game::GetInstance().activeSlide = Game::GetInstance().slideShow.size() - 1;
					MultimediaPlayer::SetUp("./Assets/audio/Ambience_MainMenu_ChiPhil.wav", true, true);
					MultimediaPlayer::GetInstance().startAudio();
					break;
				case (int)GameState::MainMenu:
					Game::GetInstance().state = GameState::InGame_Standard;
					Game::GetInstance().GenerateWorld();
					MultimediaPlayer::SetUp("./Assets/audio/Ambience_InGame_ChiPhil.wav", true, true);
					MultimediaPlayer::GetInstance().startAudio();
					break;
				case (int)GameState::Outro: case (int)GameState::Death:
					exit(0);
				}
			}
		}
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
		if (g_world)
		{
			g_world->Update();
		}
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
				slideStart = clock();
				MultimediaPlayer::GetInstance().startAudio();
			}
			else if (state == GameState::Intro)
			{
				clock_t current = clock();
				elapsed = (current - slideStart) / (double)CLOCKS_PER_SEC;
				double sum = 0;
				if (elapsed > slideShow[activeSlide].getDuration())
				{
					++activeSlide;
					slideStart = clock();
				}
				if (activeSlide == slideShow.size() - 1)
				{
					state = GameState::MainMenu;
					MultimediaPlayer::SetUp("./Assets/audio/Ambience_MainMenu_ChiPhil.wav", true, true);
					MultimediaPlayer::GetInstance().startAudio();
				}
			}
		}
	}

	void Game::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

		if (g_world && state != GameState::Outro && state != GameState::Death) { g_world->Render(); }
		else
		{
			Vector2 position = { 0.f, 0.f };
			Vector2 size = { 1.f, (float)glutGet(GLUT_WINDOW_HEIGHT) / (float)glutGet(GLUT_WINDOW_WIDTH) };
			string texture;
			if (state == GameState::Outro) texture = "ogp_end";
			else if (state == GameState::Death) texture = "ogp_lose";
			else texture = slideShow[activeSlide].getTexName();
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