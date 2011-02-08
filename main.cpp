#include <GL/glut.h>

void init (void);
void display (void);
void reshape (int, int);

int main (int argc, char **argv) {
  glutInit(&argc, argv);  // initialize GLUT

  glutInitDisplayMode (GLUT_SINGLE);  // set up a basic buffer (only single buffered)

  glutInitWindowSize (500, 500);  //set width, height of the window
  glutInitWindowPosition (100, 100);  // set position of the window

  glutCreateWindow ("Your first OpenGL Window");  // sets the title for the window

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

  init();

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

  glColor3f(0.0, 0.0, 0.0);
  glTranslatef(0.0f, 0.0f, -5.0f);
  glutSolidCube(2.0f); //render the primitive
  glColor4f(0.2, 0.2, 0.2, 0.3);
  glutSolidSphere(3.0f, 50, 50);

  glFlush();  // flush buffers to the screen
}

void reshape (int width, int height) {
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);  //set viewport to the size of the window
  glMatrixMode(GL_PROJECTION); // switch to the projection matrix so we can manipulate how the scene is viewed
  glLoadIdentity(); // Reset the projection matrix to the id matrix so we don't get any artifacts
  gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 100.0); //set the field of view angle (deg), aspect ratio of window, and near and far planes
  glMatrixMode(GL_MODELVIEW); //switch back to model-view matrix => able to draw shapes correctly
}