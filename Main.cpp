/*-----------------------------------------------------------------------------------------
 COURSE:				
 PROGRAMMERS:			Gage Coates (Coates347)
 MODIFIED BY:			Gage Coates (Coates347)
 DATE MODIFIED:			

 FOLDER:				
 DESCRIPTION:			
 NOTE:					N/A
 FILES:					
 IDE/COMPILER:			MicroSoft Visual Studio 2019
 INSTRUCTION FOR COMPILATION AND EXECUTION:
	1.		Double click on project3.sln	to OPEN the project
	2.		Press Ctrl+F7					to COMPILE
	3.		Press Ctrl+F5					to EXECUTE
-----------------------------------------------------------------------------------------*/
#include "pch.h"
#include "World.h"
#include "Vector2.h"

void initialize();
void update();
void render();
void mouseMoveHandler(int cursorX, int cursorY);
void keyHandler(unsigned char key, int x, int y);
void keyUpHandler(unsigned char key, int x, int y);

World world;

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
	glutDisplayFunc(render);
	glutPassiveMotionFunc(mouseMoveHandler);
	glutKeyboardFunc(keyHandler);
	glutKeyboardUpFunc(keyUpHandler);
	// get into an infinite loop
	glutIdleFunc(update);
	glutDisplayFunc(render);
	glutMainLoop();

	return 0;
}

void initialize() {
	// specify a background clor: white
	glClearColor(1, 1, 1, 1);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glClearDepth(1.0f);                 // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);			// Enable depth testing for z-culling
	glDepthFunc(GL_LESS);				// Set the type of depth-test
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DOUBLEBUFFER);
	//glPolygonMode(GL_BACK, GL_NONE);
	// Set the viewport to cover the new window
	glViewport(-1, 1, -1, 1);

	// Enable perspective projection with fovy, aspect, zNear and zFar
	glOrtho(-1, 1, -1, 1, -1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
void update()
{
	double elapsed = 0.0166666;

	world.Update(elapsed);
	render();
}
void render() {
	world.Render();
}

void mouseMoveHandler(int cursorX, int cursorY)
{
	world.UpdateMousePosition(Vector2(cursorX, cursorY));
}

void keyHandler(unsigned char key, int x, int y)
{
	world.UpdateKeyState(key, true);
}

void keyUpHandler(unsigned char key, int x, int y)
{
	world.UpdateKeyState(key, false);
}
