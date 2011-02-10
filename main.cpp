#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

void init (void);
void display (void);
void reshape (int, int);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void sphere_movement(void);
void create_bullet(GLfloat, GLfloat, GLfloat, GLfloat);
void bullet_movement(int id);

void draw_player_ship(void);
void draw_bullets(void);

struct entity {
	bool exists;
	GLfloat colorv[3];
	GLfloat posv[3];
	GLfloat dirv[3];
	GLfloat sizef;
};

typedef struct entity* Entity;

#define VIEW_XMAX 4.8
#define VIEW_XMIN -4.8
#define VIEW_YMAX 2.6
#define VIEW_YMIN -2.6
#define BULLETS_MAX 20
#define ZDRAW -5.0f

// global color vector for use with readPixels()
GLfloat colorVec[3] = {0.0,0.0,0.0};

GLfloat posVec[3] = { 0.0f, 0.0f, ZDRAW };
GLfloat MOVE_INCREMENT = 0.05f;

GLsizei Height = 450;
GLsizei Width = 800;

bool keyStates[256];

entity bullets[BULLETS_MAX];
int bullet_count = 0;
Entity enemies[4];

int main (int argc, char **argv) {
  glutInit(&argc, argv);  // initialize GLUT

  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // set up a basic buffer (only single buffered)

  glutInitWindowSize (Width, Height);  //set width, height of the window
  glutInitWindowPosition (100, 100);  // set position of the window

  glutCreateWindow ("Project 1: Joshua Kovach");  // sets the title for the window

  glutDisplayFunc(display);
	glutIdleFunc(display);

  glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIgnoreKeyRepeat(1);	// ignore long key presses
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouse);

  init();

	glEnable(GL_DEPTH_TEST);
  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(1.f, 1.f, 1.f, 1.f);
	int i;
	for (i = 0; i < BULLETS_MAX; i++) {
		bullets[i].exists = false;
	}

}

void display (void) {
  // clear screen to current background color
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // reset background color
  glLoadIdentity(); // load id matrix to reset drawing locations

	draw_player_ship();


	draw_bullets();

  //glFlush();  // flush buffers to the screen
	glutSwapBuffers();
}

void reshape (int width, int height) {
	// reset the globals
	Width = width;
	Height = height;
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
	float x_view, y_view;
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		x_view = (x - Width/2.0) * (VIEW_XMAX-VIEW_XMIN) / Width;
		// this line was the cause of so much trouble: always adjust for screen y
		y_view = ((Height - y) - Height/2.0) * (VIEW_YMAX-VIEW_YMIN) / Height;

		create_bullet(posVec[0], posVec[1], x_view, y_view);

		printf("Clicked at (%f, %f)\n", x_view, y_view);
		fflush(stdout);

	}
	glutPostRedisplay();
}

void draw_player_ship() {
	glPushMatrix();
		sphere_movement();
		glTranslatef(posVec[0], posVec[1], posVec[2]);
		glColor3f(0.0, 0.0, 0.0);
		glutWireSphere(0.2, 6, 6);
	glPopMatrix();
}

void sphere_movement()
{
	// move up or down
		if (keyStates['w'] || keyStates['W'])
			posVec[1] += ( posVec[1] > VIEW_YMAX ? 0 : MOVE_INCREMENT);
		if (keyStates['s'] || keyStates['S'])
			posVec[1] -= ( posVec[1] < VIEW_YMIN ? 0 : MOVE_INCREMENT);

	// move left or right
	if (keyStates['a'] || keyStates['A'])
		posVec[0] -= ( posVec[0] < VIEW_XMIN ? 0 : MOVE_INCREMENT);
	if (keyStates['d'] || keyStates['D'])
		posVec[0] += ( posVec[0] > VIEW_XMAX ? 0 : MOVE_INCREMENT);
}

void draw_bullets() {
	int i;
	for (i = 0; i < BULLETS_MAX; i++) {
		if ( bullets[i].exists != true ) {
			continue;
		}
		glPushMatrix();
			bullet_movement(i);
			glTranslatef(bullets[i].posv[0], bullets[i].posv[1], bullets[i].posv[2]);
			glColor3fv(bullets[i].colorv);
			glutSolidSphere(bullets[i].sizef, 10, 10);
		glPopMatrix();
		// printf("bullet %d moved to (%f, %f)\n", i, bullets[i].posv[0], bullets[i].posv[1]);
	}

	fflush(stdout);
}

void create_bullet(GLfloat x, GLfloat y, GLfloat x_view, GLfloat y_view) {
		Entity bullet = &bullets[bullet_count];
		bullet->exists = true;
		if (bullets[bullet_count].exists)
			printf("bullet %d created\n", bullet_count);

		// set its position as the current player position
		bullet->posv[0] = x;
		bullet->posv[1] = y;
		bullet->posv[2] = ZDRAW;
		bullet_count = (bullet_count + 1) % BULLETS_MAX;

		// set its movement vector
		GLfloat xvec = x_view - x;
		GLfloat yvec = y_view - y;
		GLfloat max = fabsf((fabsf(xvec) > fabsf(yvec)) ? xvec : yvec);
		//printf("Max: %f\n", max);

		bullet->dirv[0] = xvec / max;
		bullet->dirv[1] = yvec / max;
		bullet->dirv[2] = 0;	// not going anywhere depthwise

		bullet->colorv[0] = 1.0f;
		bullet->colorv[1] = 0.0f;
		bullet->colorv[2] = 0.0f;

		bullet->sizef = 0.05f;

		printf("bullet created at (%f, %f)", bullet->posv[0], bullet->posv[1]);
		printf(" with vector (%f, %f)\n", bullet->dirv[0], bullet->dirv[1]);

		fflush(stdout);
}

void bullet_movement(int id) {
	// changes the bullets position based on the direction vector
	Entity bullet = &bullets[id];
	bullet->posv[0] += bullet->dirv[0] * 0.1;	// x
	bullet->posv[1] += bullet->dirv[1] * 0.1;	// y

	// detect out of bounds (plus a buffer distance)
	if (bullet->posv[0] > VIEW_XMAX + 0.5 ||
			bullet->posv[0] < VIEW_XMIN - 0.5 ||
			bullet->posv[1] > VIEW_YMAX + 0.5 ||
			bullet->posv[1] < VIEW_YMIN - 0.5) {
		bullet->exists = false;
		printf("bullet %d destroyed at (%f, %f)\n", id, bullet->posv[0], bullet->posv[1]);
	}
	fflush(stdout);
}
