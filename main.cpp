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
void create_enemy(void);
void bullet_movement(int id);
void enemy_movement(int id);

int enemy_collision(int);
GLfloat distance_squared(GLfloat, GLfloat, GLfloat, GLfloat);

void draw_player_ship(void);
void draw_enemies(void);
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
#define ENEMIES_MAX 50
#define ZDRAW -5.0f
#define BULLET_SPEED 0.1
#define ENEMY_SPEED 0.01
#define MOVE_INCREMENT 0.05f	//movement of the player ship

// global color vector for use with readPixels()
GLfloat colorVec[3] = {0.0,0.0,0.0};

GLfloat posVec[3] = { 0.0f, 0.0f, ZDRAW };

GLsizei Height = 450;
GLsizei Width = 800;

bool keyStates[256];

entity bullets[BULLETS_MAX];
int bullet_count = 0;
entity enemies[ENEMIES_MAX];
int enemy_count = 0;
int enemies_alive = 0;

int spawn_delay = 500;
int spawn_timer = 0;
int wave_size = 4;

//spawning positions for enemies
GLfloat spawnVec[4][3] = { { 4.5f, 2.25f, ZDRAW },
													 { -4.5f, 2.25f, ZDRAW },
													 { -4.5f, -2.25f, ZDRAW },
													 { 4.5f, -2.25f, ZDRAW }};

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

	glEnable(GL_DEPTH_TEST | GL_LIGHTING);
  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.1f);

}

void display (void) {
  // clear screen to current background color
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // reset background color
  glLoadIdentity(); // load id matrix to reset drawing locations

	if (spawn_timer == 0 || enemies_alive == 0) {
		int i;
		for (i = 0; i < wave_size; i++) {
			create_enemy();
		}
	}
	spawn_timer = (spawn_timer + 1) % spawn_delay;

	draw_player_ship();

	draw_enemies();

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
		glColor3f(1.0, 0.5, 0.0);
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

	bullet->colorv[0] = 0.0f;
	bullet->colorv[1] = 1.0f;
	bullet->colorv[2] = 1.0f;

	bullet->sizef = 0.05f;

	printf("bullet created at (%f, %f)", bullet->posv[0], bullet->posv[1]);
	printf(" with vector (%f, %f)\n", bullet->dirv[0], bullet->dirv[1]);

	fflush(stdout);
}

void bullet_movement(int id) {
	// changes the bullets position based on the direction vector
	Entity bullet = &bullets[id];
	bullet->posv[0] += bullet->dirv[0] * BULLET_SPEED;	// x
	bullet->posv[1] += bullet->dirv[1] * BULLET_SPEED;	// y

	int hit_enemy = -1; // id of enemy that has been hit
	// detect out of bounds (plus a buffer distance)
	if (bullet->posv[0] > VIEW_XMAX + 0.5 ||
			bullet->posv[0] < VIEW_XMIN - 0.5 ||
			bullet->posv[1] > VIEW_YMAX + 0.5 ||
			bullet->posv[1] < VIEW_YMIN - 0.5) {
		bullet->exists = false;
		printf("bullet %d destroyed at (%f, %f)\n", id, bullet->posv[0], bullet->posv[1]);
	}
	else if ((hit_enemy = enemy_collision(id)) != -1) {
		enemies[hit_enemy].exists = false;
		bullet->exists = false;
	}
	fflush(stdout);
}

// I know this is bad coding, but I don't have the mental wherewithall right now
// to actually merge this with 'draw_bullets' in a 'draw_entities' method.
void draw_enemies() {
	int i;
	for (i = 0; i < ENEMIES_MAX; i++) {
		if ( enemies[i].exists != true ) {
			continue;
		}
		glPushMatrix();
			enemy_movement(i);
			glTranslatef(enemies[i].posv[0], enemies[i].posv[1], enemies[i].posv[2]);
			glColor3fv(enemies[i].colorv);
			glutSolidTorus(enemies[i].sizef / 2.0, enemies[i].sizef, 10, 10);
		glPopMatrix();
		// printf("enemies %d moved to (%f, %f)\n", i, enemies[i].posv[0], enemies[i].posv[1]);
	}
	fflush(stdout);
}

void enemy_movement(int id) {
	// changes the enemy position based on the direction vector
	Entity enemy = &enemies[id];
	enemy->posv[0] += enemy->dirv[0] * ENEMY_SPEED;	// x
	enemy->posv[1] += enemy->dirv[1] * ENEMY_SPEED;	// y

	// detect out of bounds (plus a buffer distance)
	if (enemy->posv[0] > VIEW_XMAX ||
			enemy->posv[0] < VIEW_XMIN ){
		enemy->dirv[0] *= -1; 	//reverse direction if you've hit the border
	}
	else if ( enemy->posv[1] > VIEW_YMAX ||
						enemy->posv[1] < VIEW_YMIN ) {
		enemy->dirv[1] *= -1;
	}
	fflush(stdout);
}

void create_enemy() {
	Entity enemy = &enemies[enemy_count];
	enemy->exists = true;
	enemies_alive++;
	if (enemies[enemy_count].exists)
		printf("enemy %d created\n", enemy_count);

	// set its position at a spawn point
	enemy->posv[0] = spawnVec[enemy_count%4][0];
	enemy->posv[1] = spawnVec[enemy_count%4][1];
	enemy->posv[2] = spawnVec[enemy_count%4][2];
	enemy_count = (enemy_count + 1) % ENEMIES_MAX;

	// set its movement vector to a non-zero float between 0.01 and 1.00
	GLfloat xvec = (((rand() + 1)% 200) - 100) / 100.0;
	GLfloat yvec = (((rand() + 1)% 200) - 100) / 100.0;
	//no need for normalization
	//GLfloat max = fabsf((fabsf(xvec) > fabsf(yvec)) ? xvec : yvec);
	//printf("Max: %f\n", max);

	enemy->dirv[0] = xvec;
	enemy->dirv[1] = yvec;
	enemy->dirv[2] = 0;	// not going anywhere depthwise

	enemy->colorv[0] = 1.0f;
	enemy->colorv[1] = 0.0f;
	enemy->colorv[2] = 0.0f;

	enemy->sizef = 0.1f;

	printf("enemy created at (%f, %f)", enemy->posv[0], enemy->posv[1]);
	printf(" with vector (%f, %f)\n", enemy->dirv[0], enemy->dirv[1]);

	fflush(stdout);
}

int enemy_collision(int bullet_id) {
	Entity bullet = &bullets[bullet_id];

	int i;
	for (i = 0; i < ENEMIES_MAX; i++) {
		if (enemies[i].exists == false) continue;
		if (distance_squared( enemies[i].posv[0], enemies[i].posv[1],
												  bullet->posv[0], bullet->posv[1]) <
				powf(bullet->sizef + enemies[i].sizef, 2) ) {
			return i;
		}

	}
	return -1;
}

GLfloat distance_squared(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	return powf(x1 - x2, 2) + powf(y1 - y2, 2);
}