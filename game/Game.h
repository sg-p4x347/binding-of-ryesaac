#pragma once

#include "pch.h"

#include "world/World.h"
using world::World;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "tex/Bitmap.h"
using tex::Bitmap;

#include "MultimediaPlayer.h"
#include "Slide.h"

#include <GL/glut.h>
#include <ctime>
using std::clock_t;

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
		clock_t gameStart;
		clock_t slideStart;

		short activeSlide;
		vector<Slide> slideShow = {
			Slide(8.5, "Panel_Present"),
			Slide(8, "Panel_Featuring"),
			Slide(7.75, "Panel_Title"),
			Slide(5, "Panel_BackStory"),
			Slide(5, "Panel_ToasterTalks"),
			Slide(5, "Panel_LockedAway"),
			Slide(5, "Panel_ToasterTalks"),
			Slide(5, "Panel_HoleInTheWall"),
			Slide(5, "Panel_Pursuit"),
			Slide(5, "Panel_MainMenu")
		};

		static void mouseMoveHandler(int cursorX, int cursorY);
		static void keyHandler(unsigned char key, int x, int y);
		static void keyUpHandler(unsigned char key, int x, int y);
		static void specialHandler(int key, int x, int y);
		static void specialUpHandler(int key, int x, int y);

		void GenerateWorld();
		void EnableLighting();

		void Update();
		void Render();
		void RenderQuad(Vector2 position, Vector2 size, string texture);
	};
}