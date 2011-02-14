/*
 * GeoBlaster by Joshua Kovach
 * A Geometry Wars clone(ish) written in C/C++
 * CIS 367 : Computer Graphics
 * Instructor: Greg Wolffe
 * Date: 11 February 2011
 *
 * */


#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>


void init (void);
void display (void);
void reshape (int, int);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void mouse (int button, int state, int x, int y);
void userChoice(int);

void create_player_ship(void);
void player_ship_movement(void);
void draw_player_ship(void);

void create_bullet(GLfloat, GLfloat, GLfloat, GLfloat);
void bullet_movement(int);
void draw_bullets(void);
void fire(int, int);
void empty(int, int);
void bomb(void);

void create_enemy(void);
void enemy_movement(int);
void draw_enemies(void);
GLfloat distance_squared(GLfloat, GLfloat, GLfloat, GLfloat);
int enemy_collision(int);
void kill_enemy(int);

void create_stars(void);
void move_star(int);
void draw_stars(void);

void move_entities(int);

void increase_difficulty(void);
void draw_hud(void);
void renderBitmapString(float, float, float, void*, char*);

struct ship {
	bool exists;
	int lives;
	GLfloat colorv[3];
	GLfloat posv[3];
	GLfloat sizef;
};

struct entity {
	bool exists;
	GLfloat colorv[3];
	GLfloat posv[3];
	GLfloat dirv[3];
	GLfloat sizef;
	int kill_points;
};

typedef struct entity* Entity;

#define SHIP_SIZE 0.2f
#define VIEW_XMAX 4.8
#define VIEW_XMIN -4.8
#define VIEW_YMAX 2.6
#define VIEW_YMIN -2.6
#define BULLETS_MAX 300
#define ENEMIES_MAX 500
#define ZDRAW -5.0f
#define BULLET_SPEED 0.2
#define ENEMY_SPEED 0.02
#define MOVE_INCREMENT 0.1f	//movement of the player ship
#define MOVEMENT_TIME 15
#define BOMB_SPEED 2.0
#define STARS 100

GLsizei Height = 450;
GLsizei Width = 800;

bool keyStates[256];

struct ship player_ship;

entity bullets[BULLETS_MAX];
int bullet_count = 0;

entity enemies[ENEMIES_MAX];
int enemy_count;
int enemies_alive;

//spawning positions for enemies
GLfloat spawnVec[4][3] = { { 4.5f, 2.25f, ZDRAW },
													 { -4.5f, 2.25f, ZDRAW },
													 { -4.5f, -2.25f, ZDRAW },
													 { 4.5f, -2.25f, ZDRAW }};
int spawn_delay;
int spawn_timer;
char spawn_timer_s[64];
int wave_size;

int bombs;
bool bombing;

int points;
char points_s[64];

entity stars[STARS];

int main (int argc, char **argv) {
  glutInit(&argc, argv);  // initialize GLUT

  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // set up a basic buffer (only single buffered)

  glutInitWindowSize (Width, Height);  //set width, height of the window
  glutInitWindowPosition (100, 100);  // set position of the window

  glutCreateWindow ("GeoBlaster by Joshua Kovach");  // sets the title for the window

  glutDisplayFunc(display);
	//glutIdleFunc(display);

  glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIgnoreKeyRepeat(1);	// ignore long key presses
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouse);
	glutTimerFunc(MOVEMENT_TIME, move_entities, 0);

	int spawnRateMenu = glutCreateMenu(userChoice);
		glutAddMenuEntry("Increase", 1);
		glutAddMenuEntry("Decrease", 2);

	int colorMenu = glutCreateMenu(userChoice);
		glutAddMenuEntry("Orange", 4);
		glutAddMenuEntry("Blue", 5);
		glutAddMenuEntry("Green", 6);
		glutAddMenuEntry("Yellow",7);


	int menu = glutCreateMenu(userChoice);
		glutAddMenuEntry("Reset game", 0);
		glutAddSubMenu("Spawn Rate", spawnRateMenu);
		glutAddSubMenu("Ship Color", colorMenu);
		glutAddMenuEntry("Special Weapon", 3);
		glutAddMenuEntry("Standard Weapon", 8);

		glutAttachMenu(GLUT_RIGHT_BUTTON);

  init();

	glEnable(GL_DEPTH_TEST | GL_LIGHTING);
  glutMainLoop();

  return 0;
}

void init(void) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	create_player_ship();
	create_stars();

	// clear all enemies
	for (int i = 0; i < ENEMIES_MAX; i++) {
		enemies[i].exists = false;
	}

	glutMotionFunc(empty);	//standard fire mode
	enemy_count = 0;
	enemies_alive = 0;
	spawn_timer = 0;
	spawn_delay = 500;
	wave_size = 4;
	bombs = 3;
	bombing = false;
	points = 0;
}

void display (void) {
  // clear screen to current background color
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // load id matrix to reset drawing locations

	increase_difficulty();	// check for difficulty change based on points

	if (player_ship.exists == false) {	// player has died
		glClearColor(0.8, 0.0, 0.0, 0.5);
		glColor3f(0.0, 0.0, 0.0);
		renderBitmapString (-3.0, 0.5, ZDRAW, GLUT_BITMAP_HELVETICA_18,
											"YOUR SHIP HAS BEEN DESTROYED! Press '/' to restart");
	}



	draw_hud();
	draw_stars();
	draw_player_ship();
	draw_enemies();
	draw_bullets();

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
		case ('/'):	// reset the game
			init();
			break;
		case (' '):	// bomb
			bomb();
			break;
		case ('`'):	// special weaopn
			glutMotionFunc(fire);
			break;
		case ('1'):	// standard weapon
			glutMotionFunc(empty);
			break;
		default:
			break;
	}

	keyStates[key] = true;	// for long-press controls
	//glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
	keyStates[key] = false;	// key not down
}

void mouse (int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) fire(x, y);
	//glutPostRedisplay();
}

void fire(int x, int y) {
	float x_view, y_view;
	x_view = (x - Width/2.0) * (VIEW_XMAX-VIEW_XMIN) / Width;
	// this line was the cause of so much trouble: always adjust for screen y
	y_view = ((Height - y) - Height/2.0) * (VIEW_YMAX-VIEW_YMIN) / Height;

	create_bullet(player_ship.posv[0], player_ship.posv[1], x_view, y_view);
}

void bomb() {
	if (bombs > 0) {
		bombing = true;
		GLfloat xvec, yvec;

		for (int i = 0; i < BULLETS_MAX; i++) {
			xvec = (((rand() + 1)% Width));
			yvec = (((rand() + 1)% Height));
			fire(xvec, yvec);
		}

		bombs--;
		bombing = false;
	}
}

void draw_player_ship() {
	if (player_ship.exists) {
		glPushMatrix();
			// player_ship_movement();
			glTranslatef(player_ship.posv[0], player_ship.posv[1], player_ship.posv[2]);
			glColor3f(player_ship.colorv[0], player_ship.colorv[1], player_ship.colorv[2]);
			glutWireSphere(SHIP_SIZE, 4, 4);
		glPopMatrix();
	}
}

void player_ship_movement()
{
	// move up or down
		if (keyStates['w'] || keyStates['W'])
			player_ship.posv[1] += ( player_ship.posv[1] > VIEW_YMAX ? 0 : MOVE_INCREMENT);
		if (keyStates['s'] || keyStates['S'])
			player_ship.posv[1] -= ( player_ship.posv[1] < VIEW_YMIN ? 0 : MOVE_INCREMENT);

	// move left or right
	if (keyStates['a'] || keyStates['A'])
		player_ship.posv[0] -= ( player_ship.posv[0] < VIEW_XMIN ? 0 : MOVE_INCREMENT);
	if (keyStates['d'] || keyStates['D'])
		player_ship.posv[0] += ( player_ship.posv[0] > VIEW_XMAX ? 0 : MOVE_INCREMENT);
}

void draw_bullets() {
	for (int i = 0; i < BULLETS_MAX; i++) {
		if ( bullets[i].exists != true ) {
			continue;
		}
		glPushMatrix();
			// bullet_movement(i);
			glTranslatef(bullets[i].posv[0], bullets[i].posv[1], bullets[i].posv[2]);
			glColor3fv(bullets[i].colorv);
			glutWireSphere(bullets[i].sizef, 3, 3);
		glPopMatrix();
	}
}

void create_bullet(GLfloat x, GLfloat y, GLfloat x_view, GLfloat y_view) {
	Entity bullet = &bullets[bullet_count];
	bullet->exists = true;

	// set its position as the current player position
	bullet->posv[0] = x;
	bullet->posv[1] = y;
	bullet->posv[2] = ZDRAW;
	bullet_count = (bullet_count + 1) % BULLETS_MAX;

	// set its movement vector (normalized)
	GLfloat xvec = x_view - x;
	GLfloat yvec = y_view - y;
	GLfloat max = fabsf((fabsf(xvec) > fabsf(yvec)) ? xvec : yvec);

	bullet->dirv[0] = xvec / max;
	bullet->dirv[1] = yvec / max;
	bullet->dirv[2] = 0;	// not going anywhere depthwise

	bullet->colorv[0] = 0.0f;
	bullet->colorv[1] = 1.0f;
	bullet->colorv[2] = 1.0f;

	bullet->sizef = 0.05f;

	bullet->kill_points = 0;

	//printf("bullet created at (%f, %f)", bullet->posv[0], bullet->posv[1]);
	//printf(" with vector (%f, %f)\n", bullet->dirv[0], bullet->dirv[1]);
	//fflush(stdout);
}

void bullet_movement(int id) {
	// changes the bullets position based on the direction vector
	Entity bullet = &bullets[id];
	bullet->posv[0] += bullet->dirv[0] * (bombing ? BOMB_SPEED : BULLET_SPEED);	// x
	bullet->posv[1] += bullet->dirv[1] * (bombing ? BOMB_SPEED : BULLET_SPEED);	// y

	int hit_enemy = -1; // id of enemy that has been hit
	// detect out of bounds (plus a buffer distance)
	if (bullet->posv[0] > VIEW_XMAX + 0.5 ||
			bullet->posv[0] < VIEW_XMIN - 0.5 ||
			bullet->posv[1] > VIEW_YMAX + 0.5 ||
			bullet->posv[1] < VIEW_YMIN - 0.5) {
		bullet->exists = false;
		//printf("bullet %d destroyed at (%f, %f)\n", id, bullet->posv[0], bullet->posv[1]);
	}
	else if ((hit_enemy = enemy_collision(id)) != -1) {
		kill_enemy(hit_enemy);
		bullet->exists = false;
	}
	//fflush(stdout);
}

// I know this is bad coding, but I don't have the mental wherewithall right now
// to actually merge this with 'draw_bullets' in a 'draw_entities' method.
void draw_enemies() {
	for (int i = 0; i < ENEMIES_MAX; i++) {
		if ( enemies[i].exists != true ) {
			continue;
		}
		glPushMatrix();
			//enemy_movement(i);
			glTranslatef(enemies[i].posv[0], enemies[i].posv[1], enemies[i].posv[2]);
			glColor3fv(enemies[i].colorv);
			glutWireTorus(enemies[i].sizef / 2.0, enemies[i].sizef, 10, 5);
		glPopMatrix();
		// printf("enemies %d moved to (%f, %f)\n", i, enemies[i].posv[0], enemies[i].posv[1]);
	}
	//fflush(stdout);
}

void enemy_movement(int id) {
	// changes the enemy position based on the direction vector
	Entity enemy = &enemies[id];
	enemy->posv[0] += enemy->dirv[0] * ENEMY_SPEED;	// x
	enemy->posv[1] += enemy->dirv[1] * ENEMY_SPEED;	// y

	// detect out of bounds (plus a buffer distance)
	if (enemy->posv[0] > VIEW_XMAX || enemy->posv[0] < VIEW_XMIN ){
		enemy->dirv[0] *= -1; 	//reverse direction if you've hit the border
	}
	else if ( enemy->posv[1] > VIEW_YMAX || enemy->posv[1] < VIEW_YMIN ) {
		enemy->dirv[1] *= -1;
	}
	else if ( distance_squared(enemy->posv[0], enemy->posv[1],
														 player_ship.posv[0], player_ship.posv[1]) <
					 powf(enemy->sizef + SHIP_SIZE, 2)) {
		kill_enemy(id);
		player_ship.lives--;
		if (player_ship.lives <= 0) player_ship.exists = false;
	}
}

void create_enemy() {
	Entity enemy = &enemies[enemy_count];
	enemy->exists = true;
	enemies_alive++;

	// set its position at a spawn point
	enemy->posv[0] = spawnVec[enemy_count%4][0];
	enemy->posv[1] = spawnVec[enemy_count%4][1];
	enemy->posv[2] = spawnVec[enemy_count%4][2];
	enemy_count = (enemy_count + 1) % ENEMIES_MAX;

	// set its movement vector to a non-zero float between 0.01 and 1.00
	GLfloat xvec = (((rand() + 1)% 200) - 100) / 100.0;
	GLfloat yvec = (((rand() + 1)% 200) - 100) / 100.0;

	enemy->dirv[0] = xvec * (rand() % 3) + 1;
	enemy->dirv[1] = yvec * (rand() % 3) + 1;
	enemy->dirv[2] = 0;	// not going anywhere depthwise

	enemy->colorv[0] = 0.2 + (rand() % 100) / 100.0;	//random color, not black
	enemy->colorv[1] = 0.2 + (rand() % 100) / 100.0;
	enemy->colorv[2] = 0.2 + (rand() % 100) / 100.0;

	enemy->sizef = 0.1f;
	enemy->kill_points = 100;

	//printf("enemy created at (%f, %f)", enemy->posv[0], enemy->posv[1]);
	//printf(" with vector (%f, %f)\n", enemy->dirv[0], enemy->dirv[1]);
	//fflush(stdout);
}

int enemy_collision(int bullet_id) {
	Entity bullet = &bullets[bullet_id];
	for (int i = 0; i < ENEMIES_MAX; i++) {
		if (enemies[i].exists == false) continue;
		if (distance_squared( enemies[i].posv[0], enemies[i].posv[1],
												  bullet->posv[0], bullet->posv[1]) <=
				powf(bullet->sizef + enemies[i].sizef, 2) ) {
			return i;
		}
	}
	return -1;
}

GLfloat distance_squared(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	return powf(x1 - x2, 2) + powf(y1 - y2, 2);
}

void create_player_ship() {
	player_ship.exists = true;

	player_ship.posv[0] = 0;
	player_ship.posv[1] = 0;
	player_ship.posv[2] = ZDRAW;

	player_ship.lives = 3;

	player_ship.colorv[0] = 1.0f;
	player_ship.colorv[1] = 0.5f;
	player_ship.colorv[2] = 0.0f;
}

void kill_enemy(int id) {
	enemies[id].exists = false;
	if (player_ship.exists) points += enemies[id].kill_points;
	enemies_alive--;
}

void increase_difficulty() {
	if (points % 1000 == 0 && points != 0) {
		wave_size++;
		points += 100;
	}
	if (points % 10000 == 0 && points != 0) {
		player_ship.lives++;
		points += 100;
	}
}

void draw_hud() {
	for (int i = 0; i < player_ship.lives; i++) {
		glPushMatrix();	// draw lives
			glTranslatef(3.8-i*0.25, 2.0, ZDRAW + 1);
			glColor3f(1.0, 0.1, 0.1);
			glutWireSphere(0.075, 6, 6);
		glPopMatrix();
	}

	for (int i = 0; i < bombs; i++) {
		glPushMatrix(); 	// Draw bombs
			glTranslatef(3.8-i*0.25, -2.0, ZDRAW + 1);
			glColor3f(0.1, 1.0, 0.1);
			glutWireCone(0.075, 0.2, 10, 5);
		glPopMatrix();
	}

	sprintf(points_s,"PTS: %d", points);
	renderBitmapString (-3.8, 2.0, ZDRAW + 1, GLUT_BITMAP_HELVETICA_18, points_s);

	sprintf(spawn_timer_s, "SPAWN IN %g", (spawn_delay-spawn_timer) / 100.0);
	renderBitmapString (-3.8, -2.0, ZDRAW + 1, GLUT_BITMAP_HELVETICA_18, spawn_timer_s);
}

/* taken from examples online */
void renderBitmapString(float x, float y, float z, void *font, char *string) {
	char *c;
	glRasterPos3f(x, y, z);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

// empty string for resetting mouse functions
void empty(int x, int y) {}

void userChoice(int option) {
	switch (option) {
		case 0:	// Reset Game
			init();
			break;
		case 1:	// Increase spawn rate
			spawn_delay -= (spawn_delay == 50) ? 0 : 50;
			break;
		case 2:	// Decrease spawn rate
			spawn_delay += (spawn_delay == 1000) ? 0 : 50;
			break;
		case 3:	// Special Weapon
			glutMotionFunc(fire);
			break;
		case 8: // Regular weapon
			glutMotionFunc(empty);
			break;
		case 4:	// Ship orange
			player_ship.colorv[0] = 1.0;
			player_ship.colorv[1] = 0.5;
			player_ship.colorv[2] = 0.0;
			break;
		case 5:	// Ship Blue
			player_ship.colorv[0] = 0.1;
			player_ship.colorv[1] = 0.1;
			player_ship.colorv[2] = 1.0;
			break;
		case 6:	// Ship Green
			player_ship.colorv[0] = 0.1;
			player_ship.colorv[1] = 1.0;
			player_ship.colorv[2] = 0.1;
			break;
		case 7:	// Ship Yellow
			player_ship.colorv[0] = 1.0;
			player_ship.colorv[1] = 1.0;
			player_ship.colorv[2] = 0.1;
			break;
		default:
			break;
	}
}

void draw_stars() {
	for (int i = 0; i < STARS; i++) {
		//move_star(i);
		glPushMatrix();
			glTranslatef(stars[i].posv[0], stars[i].posv[1], stars[i].posv[2]);
			glColor3f(1.0, 1.0, 1.0);
			glutSolidSphere(0.01, 3, 3);
		glPopMatrix();
	}
}

void create_stars() {
	for (int i = 0; i < STARS; i++) {
		stars[i].posv[0] = ((rand() % 200) - 100) / 15.0;
		stars[i].posv[1] = ((rand() % 200) - 100) / 30.0;
		stars[i].posv[2] = ZDRAW - 1.0;
		stars[i].dirv[0] = (((rand() + 1)% 200) - 100) / 100.0;
		stars[i].dirv[1] = (((rand() + 1)% 200) - 100) / 100.0;
	}
}

void move_star(int i) {
	stars[i].posv[0] += stars[i].dirv[0] * 0.001;
	stars[i].posv[1] += stars[i].dirv[1] * 0.001;
}

void move_entities(int value) {
	player_ship_movement();
	for (int i = 0; i < BULLETS_MAX; i++) {
		if ( bullets[i].exists != true ) {
			continue;
		}
		bullet_movement(i);
	}

	for (int i = 0; i < ENEMIES_MAX; i++) {
		if ( enemies[i].exists != true ) {
			continue;
		}
		enemy_movement(i);
	}

	for (int i = 0; i < STARS; i++) {
		move_star(i);
	}

	if (spawn_timer == 0) {
		for (int i = 0; i < wave_size; i++) {
			create_enemy();
		}
	}
	spawn_timer = (spawn_timer + 1) % spawn_delay; 	// reset counter

	glutPostRedisplay();
	glutTimerFunc(MOVEMENT_TIME, move_entities, 0);
}