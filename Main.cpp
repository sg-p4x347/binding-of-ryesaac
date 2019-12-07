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

#include "world/World.h"
using world::World;

#include "math/Vector2.h"
using math::Vector2;

#include "tex/TextureRepository.h"
using tex::TextureRepository;

#include "tex/Bitmap.h"
using tex::Bitmap;

#include "MultimediaPlayer.h"

#include <GL/glut.h>

#include <thread>

void initialize();
void runIntro();
void update();
void render();
void renderTick(int value);
void mouseMoveHandler(int cursorX, int cursorY);
void keyHandler(unsigned char key, int x, int y);
void keyUpHandler(unsigned char key, int x, int y);
void specialHandler(int key, int x, int y);
void specialUpHandler(int key, int x, int y);

World g_world;

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
	glutPassiveMotionFunc(mouseMoveHandler);
	glutKeyboardFunc(keyHandler);
	glutKeyboardUpFunc(keyUpHandler);
	glutSpecialFunc(specialHandler);
	glutSpecialUpFunc(specialUpHandler);
	glutIdleFunc(update);
	glutDisplayFunc(render);

	g_world.Generate();
	// Start 60 Hz frame rendering
	glutTimerFunc(16, renderTick,0);
	glutMainLoop();

	return 0;
}

void initialize() {
	// specify a background clor: white
	glClearColor(1, 1, 1, 1);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glClearDepth(1.0f);                 // Set background depth to farthest
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);			// Enable depth testing for z-culling
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	float pos[]{1.f,1.f,0.f,1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	float spotDir[]{ 0.f,-1.f,0.f };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
	float ambient[]{ 0.5f,0.5f,0.5f,1.f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
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

	runIntro();
}
void runIntro()
{
	//auto bitmap = TextureRepository::GetBitmap("myBitmap");
	//glDrawPixels(bitmap->GetWidth(), bitmap->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, bitmap->GetPixels());
	// Set up audio
	MultimediaPlayer musicPlayer = MultimediaPlayer("./Assets/audio/Intro_Condesa_Vox_Overlay.wav", true, false);
	musicPlayer.startAudio();
}
void update()
{
	g_world.Update();
}
void render() 
{
	g_world.Render();
}

void renderTick(int value)
{
	glutPostRedisplay();
	glutTimerFunc(16,renderTick,0);
}

void mouseMoveHandler(int cursorX, int cursorY)
{
	g_world.UpdateMousePosition(Vector2(cursorX, cursorY));
}

void keyHandler(unsigned char key, int x, int y)
{
	g_world.UpdateKeyState(key, true);
}

void keyUpHandler(unsigned char key, int x, int y)
{
	g_world.UpdateKeyState(key, false);
}

void specialHandler(int key, int x, int y)
{
	g_world.UpdateSpecialKeyState(key, true);
}

void specialUpHandler(int key, int x, int y)
{
	g_world.UpdateSpecialKeyState(key, false);
}
