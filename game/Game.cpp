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
		GetInstance().g_world = std::make_unique<World>();
		if (GetInstance().g_world)
			GetInstance().g_world->Generate();
	}

	void Game::Update()
	{
		if (g_world) g_world->Update();
	}

	void Game::Render()
	{
		if (g_world) { g_world->Render(); }
		else
		{
			auto introPanels = TextureRepository::GetBitmap("intro_corrected");
			gluOrtho2D(0, 1, 0, 1);
			glBegin(GL_BITMAP);
			glRasterPos2d(0, 0);
			glDrawPixels(introPanels->GetWidth() / 2, introPanels->GetHeight() / 2, GL_RGBA, GL_UNSIGNED_BYTE, introPanels->GetPixels());
			glEnd();
		}
	}
}