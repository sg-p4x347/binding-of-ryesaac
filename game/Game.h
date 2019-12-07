#pragma once

#include "pch.h"

#include "world/World.h"
using world::World;

namespace game
{
	class Game
	{
	private:
		Game();
		unique_ptr<World> g_world;
	public:
		static Game& GetInstance();
		static void mouseMoveHandler(int cursorX, int cursorY);
		static void keyHandler(unsigned char key, int x, int y);
		static void keyUpHandler(unsigned char key, int x, int y);
		static void specialHandler(int key, int x, int y);
		static void specialUpHandler(int key, int x, int y);

		void GenerateWorld();

		void Update();
		void Render();
	};
}