// myBucket.c
//
// demonstrates color bucket choice

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.141592

// global color vector
GLfloat colorVec[3] = {0.0,0.0,0.0};
int Height = 500;
int Width = 500;

// prototypes
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void setColor (int x, int y);
void drawCircle (int centerX, int centerY, float radius);
void drawSquare (void);


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (Width, Height);
  glutInitWindowPosition (100, 100);
  glutCreateWindow ("CIS 367 - color buckets");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc (keyboard);
  glutMouseFunc (mouse);
  glutMainLoop();
  return 0;
}


void init(void)
{
// set background color, setup viewing, give instructions to user
   glClearColor (0.0, 0.0, 0.0, 0.0);

   glViewport(0, 0, 500, 500);
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   gluOrtho2D (0.0, 500.0, 0.0, 500.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity();

   printf ("Click on color bucket to display square of that color.\n");
}


void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);

	 glColor3f (1.0f, 1.0f, 1.0f);	// white
   drawCircle (25, 475, 25);

	 glColor3f (1.0f, 0.0f, 0.0f);	// red
   drawCircle (25, 425, 25);

	 glColor3f (0.0f, 1.0f, 0.0f); 	// green
   drawCircle (25, 375, 25);

	 glColor3f (0.0f, 0.0f, 1.0f);	// blue
   drawCircle (25, 325, 25);

	 glColor3f (0.0f, 1.0f, 1.0f);	// cyan
   drawCircle (25, 275, 25);

	 glColor3f (1.0f, 1.0f, 0.0f);	// yellow
   drawCircle (25, 225, 25);

	 glColor3f (1.0f, 0.0f, 1.0f);	// magenta
   drawCircle (25, 175, 25);

   glColor3fv (colorVec);
   drawSquare();
   glFlush();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}


void mouse (int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		glReadPixels(x, Height - y, 1, 1, GL_RGB, GL_FLOAT, colorVec);
		setColor (x, y);
	}
	glutPostRedisplay();
}


void setColor (int x, int y)
{
	if (x<=50 && y<50) {
		colorVec[0] = 1.0;
		colorVec[1] = 1.0;
		colorVec[2] = 1.0;
	}
}


void drawCircle (int centerX, int centerY, float radius)
{
	int i;
	float angle;

	glBegin (GL_POLYGON);
		for (i=0; i<100; i++) {
			angle = 2*PI*i / 100;
			glVertex2f(centerX + radius*cos(angle), centerY + radius*sin(angle));
		}
	glEnd();
}


void drawSquare (void)
{
   glBegin(GL_POLYGON);
      glVertex2f(175.0, 175.0);
      glVertex2f(325.0, 175.0);
      glVertex2f(325.0, 325.0);
      glVertex2f(175.0, 325.0);
   glEnd();
}
