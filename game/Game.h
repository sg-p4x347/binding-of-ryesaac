#pragma once

#include "pch.h"

#include "world/World.h"
using world::World;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "tex/Bitmap.h"
using tex::Bitmap;

#include "MultimediaPlayer.h"

#include <GL/glut.h>

enum GameState {
	None,
	Intro,
	MainMenu,
	InGame_Standard,
	InGame_BossBattle,
	Outro
};

namespace game
{
	class Game
	{
	private:
		Game();
		unique_ptr<World> g_world;
	public:
		static Game& GetInstance();
		GameState state = GameState::None;

		shared_ptr<Bitmap> introPanels = TextureRepository::GetBitmap("intro_scaled");

		static void mouseMoveHandler(int cursorX, int cursorY);
		static void keyHandler(unsigned char key, int x, int y);
		static void keyUpHandler(unsigned char key, int x, int y);
		static void specialHandler(int key, int x, int y);
		static void specialUpHandler(int key, int x, int y);

		void GenerateWorld();

		void Update();
		void Render();
		void RenderQuad(Vector2 position, Vector2 size, string texture);
	};
}