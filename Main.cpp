/*-----------------------------------------------------------------------------------------
 COURSE:				CSC525-001 Computer Graphics
 PROGRAMMERS:			Gage Coates		(Coates347)
							Contributions:	
							Percentage:		
						Paul Durham		(Durham321)
							Contributions:
							Percentage:
						Anthony Harris	(Anthony999)
							Contributions:
							Percentage:
						Devlyn Hogue	(Hogue3)
							Contributions:
							Percentage:
 LAST MODIFIED:			12/09/2019
 DESCRIPTION:			A bread pun based version of the game "The Binding of Isaac" using
						the OpenGL graphics library
 IDE/COMPILER:			MicroSoft Visual Studio 2019
 INSTRUCTION FOR COMPILATION AND EXECUTION:
	1.		Double click on project3.sln	to OPEN the project
	2.		Press Ctrl+F7					to COMPILE
	3.		Press Ctrl+F5					to EXECUTE
-----------------------------------------------------------------------------------------*/
#include "pch.h"

#include "game/Game.h"
using game::Game;

#include "game/MultimediaPlayer.h"
using game::MultimediaPlayer;

#include "world/World.h"
using world::World;

#include "math/Vector2.h"
using math::Vector2;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "tex/Bitmap.h"
using tex::Bitmap;

#include <GL/glut.h>

#include <thread>

void initialize();
void update();
void render();
void renderTick(int value);

int main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	// specify a window size
	glutInitWindowSize(600, 600);
	// specify a window position
	glutInitWindowPosition(100, 0);
	// create a titled window
	glutCreateWindow("Binding of Ryesaac");
	// setting up
	initialize();
	// register a callback
	glutPassiveMotionFunc(Game::mouseMoveHandler);
	glutKeyboardFunc(Game::keyHandler);
	glutKeyboardUpFunc(Game::keyUpHandler);
	glutSpecialFunc(Game::specialHandler);
	glutSpecialUpFunc(Game::specialUpHandler);
	glutIdleFunc(update);
	glutDisplayFunc(render);

	//Game::GetInstance().GenerateWorld();
	// Start 60 Hz frame rendering
	glutTimerFunc(16, renderTick,0);
	glutMainLoop();

	return 0;
}

void initialize() {
	glClearColor(0,0,0, 1);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glClearDepth(1.0f);                 // Set background depth to farthest
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);			// Enable depth testing for z-culling
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);				// Set the type of depth-test
	glEnable(GL_DOUBLEBUFFER);
	glEnable(GL_TEXTURE_2D);
	// Set the viewport to cover the new window
	glViewport(-2, 2, -2, 2);

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void update()
{
	Game::GetInstance().Update();
}
void render() 
{
	Game::GetInstance().Render();
}

void renderTick(int value)
{
	glutPostRedisplay();
	glutTimerFunc(16,renderTick,0);
}