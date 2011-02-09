#include <GL/glut.h>
#include <stdio.h>

void init (void);
void display (void);
void reshape (int, int);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void sphere_movement(void);



// global color vector for use with readPixels()
GLfloat colorVec[3] = {0.0,0.0,0.0};

GLfloat posVec[3] = { 0.0f, 0.0f, -5.0f };
GLfloat MOVE_INCREMENT = 0.005f;

bool keyStates[256];

int main (int argc, char **argv) {
  glutInit(&argc, argv);  // initialize GLUT

  glutInitDisplayMode (GLUT_SINGLE);  // set up a basic buffer (only single buffered)

  glutInitWindowSize (800, 450);  //set width, height of the window
  glutInitWindowPosition (100, 100);  // set position of the window

  glutCreateWindow ("Project 1: Joshua Kovach");  // sets the title for the window

  glutDisplayFunc(display);
	glutIdleFunc(display);

  glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIgnoreKeyRepeat(1);	// ignore long key presses
	glutKeyboardUpFunc(keyboardUp);

  init();

	glEnable(GL_DEPTH_TEST);
  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(1.f, 1.f, 1.f, 1.f);

}

void display (void) {
  // clear screen to current background color
	glClear (GL_COLOR_BUFFER_BIT);  // reset background color
  glLoadIdentity(); // load id matrix to reset drawing locations

	sphere_movement();

	glTranslatef(posVec[0], posVec[1], posVec[2]);

	glColor3f(0.0, 0.0, 0.0);
	glutWireSphere(0.2, 6, 6);


  glFlush();  // flush buffers to the screen
}

void reshape (int width, int height) {
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);  //set viewport to the size of the window
  glMatrixMode(GL_PROJECTION); // switch to the projection matrix so we can manipulate how the scene is viewed
  glLoadIdentity(); // Reset the projection matrix to the id matrix so we don't get any artifacts
  gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 100.0); //set the field of view angle (deg), aspect ratio of window, and near and far planes
  glMatrixMode(GL_MODELVIEW); //switch back to model-view matrix => able to draw shapes correctly
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case (27):// esc
			exit(0);
			break;

		default:
			break;
	}

	keyStates[key] = true;
	// printf("%c-down\n", key);
	fflush(stdout);

	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
	keyStates[key] = false;
	// printf("%c-up\n", key);
	fflush(stdout);
}

void mouse (int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		glReadPixels(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 1, 1, GL_RGB, GL_FLOAT, colorVec);

	}
	glutPostRedisplay();
}

void sphere_movement()
{
	// move up or down
		if (keyStates['w'] || keyStates['W'])
			posVec[1] += ( posVec[1] > 3 ? 0 : MOVE_INCREMENT);
		if (keyStates['s'] || keyStates['S'])
			posVec[1] -= ( posVec[1] < -3 ? 0 : MOVE_INCREMENT);

	// move left or right
	if (keyStates['a'] || keyStates['A'])
		posVec[0] -= ( posVec[0] < -5 ? 0 : MOVE_INCREMENT);
	if (keyStates['d'] || keyStates['D'])
		posVec[0] += ( posVec[0] > 5 ? 0 : MOVE_INCREMENT);
}