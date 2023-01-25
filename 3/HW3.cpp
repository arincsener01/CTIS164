/*********
   CTIS164 - Template Source Program
----------
STUDENT : Arınç Doğan Şener
SECTION : 1
HOMEWORK: 3
----------
PROBLEMS: Couldn't switch off the objects. Second object takes First objects color and the third object takes the second ones color. But I properly display the on/off switch.
----------
ADDITIONAL FEATURES: Up and down key for dimming and brighten it up.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "vec.h"

#define PI 3.1415

// Action time surpassed since its start in milisec. 
#define NOW glutGet(GLUT_ELAPSED_TIME)
#define TIMER_PERIOD	10

#define TOTAL_WIDTH		800
#define TOTAL_HEIGHT	700

#define WINDOW_LEFT		-TOTAL_WIDTH / 2
#define WINDOW_RIGHT	-1 * WINDOW_LEFT
#define WINDOW_TOP		TOTAL_HEIGHT / 2
#define WINDOW_BOTTOM	-1 * WINDOW_TOP

#define BOTTOM_BAR_HEIGHT		50

#define GAME_AREA_BOTTOM		(WINDOW_BOTTOM + BOTTOM_BAR_HEIGHT)

#define GAME_AREA_MIN			{WINDOW_LEFT, GAME_AREA_BOTTOM}
#define GAME_AREA_MAX			{WINDOW_RIGHT, WINDOW_TOP}

#define FIRST_ORBIT_RADIUS		100
#define ORBIT_RADIUS_INCREMENT	100

#define PLANET_MAX_RADIUS	50
#define PLANET_MIN_RADIUS	10

#define SUN_RADIUS		20
#define LIGHT_RADIUS	10

#define ORBIT_COUNT		3

#define MAX_ANGULAR_VELOCITY	150
#define	MIN_ANGULAR_VELOCITY	0

#define MIN_LIGHT_SPEED			100
#define MAX_LIGHT_SPEED			500

#define INITIAL_LIGHT_RANGE		500

typedef struct
{
	double radius;
	double angularVel;
	vec_t position;
} planet_t;

typedef struct
{
	float r, g, b;
} color_t;

typedef struct {
	vec_t pos;
	vec_t normal_pos;
} vertex_t;

typedef struct
{
	double radius;
	bool active;
	color_t color;
	vec_t position;
	vec_t velocity;
} light_t;

int plWindowWidth, plWindowHeight;

int plTime;
bool plRun;

planet_t planets[ORBIT_COUNT];
light_t plLights[4];
double plLightRange = INITIAL_LIGHT_RANGE;

// converts value from (xmin-xmax) to (ymin-ymax) 
double rangeConversion(double value, double xmin, double xmax, double ymin, double ymax)
{
	return (((value - xmin) * (ymax - ymin)) / (xmax - xmin)) + ymin;
}

void drawCircle(int x, int y, int r)
{
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void drawCircleWire(int x, int y, int r)
{
	float angle;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}
double randomDouble(double min, double max)
{
	return rangeConversion(rand(), 0, RAND_MAX, min, max);
}

// returns 'vector' rotated by 'angle' degrees
vec_t rotate(vec_t vector, double angle)
{
	double x = cos(angle * V_D2R) * vector.x - sin(angle * V_D2R) * vector.y;
	double y = sin(angle * V_D2R) * vector.x + cos(angle * V_D2R) * vector.y;
	vector.x = x;
	vector.y = y;
	return vector;
}

double normalizeAngle(double angle)
{
	while (angle > 360)
	{
		angle -= 360;
	}

	while (angle < 0)
	{
		angle += 360;
	}

	return angle;
}

// returns rotation of vector, from +x axis, in degrees
double rotation(vec_t vector)
{
	return normalizeAngle(atan2(vector.y, vector.x) / V_D2R);
}

vec_t makeVector(double x, double y)
{
	vec_t v;
	v.x = x;
	v.y = y;
	return v;
}

double clamp(double val, double min, double max)
{
	if (val < min)
	{
		return min;
	}
	else if (val > max)
	{
		return max;
	}
	else
	{
		return val;
	}
}

void setVertex(vec_t vector)
{
	glVertex2d(vector.x, vector.y);
}

void setColor(color_t c)
{
	glColor3f(c.r, c.g, c.b);
}

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

double distanceImpact(double d) {
	return (-1.0 / plLightRange) * d + 1.0;
}

double clampAbove0(double value)
{
	return value < 0 ? 0 : value;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t dist = subV(source.position, v.pos);
	vec_t distNormalized = unitV(dist);
	float factor = clampAbove0(dotP(distNormalized, v.normal_pos)) * clampAbove0(distanceImpact(magV(dist)));
	return mulColor(factor, source.color);
}

color_t calculateTotalColor(vertex_t v)
{
	color_t buffer = { 0, 0, 0 };

	for (int i = 0; i < 4; i++)
	{
		if (plLights[i].active)
		{
			buffer = addColor(buffer, calculateColor(plLights[i], v));
		}
	}

	return buffer;
}
void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}
void updatePlanet(planet_t* cel, double deltaTime)
{
	cel->position = rotate(cel->position, cel->angularVel * deltaTime / 1000);
}
void updatePlanets(double deltaTime)
{
	for (int i = 0; i < ORBIT_COUNT; i++)
	{
		updatePlanet(&planets[i], deltaTime);
	}
}
void updateLights(double deltaTime)
{
	for (int i = 0; i < 4; i++)
	{
		plLights[i].position = addV(plLights[i].position, mulV(deltaTime / 1000, plLights[i].velocity));
	}
}
bool checkLightVertCol(const light_t light)
{
	double x = light.position.x;
	double r = light.radius;

	if (x - r <= WINDOW_LEFT || x + r >= WINDOW_RIGHT)
	{
		return true;
	}

	return false;
}
bool checkLightHorzCol(const light_t light)
{
	double y = light.position.y;
	double r = light.radius;

	if (y - r <= GAME_AREA_BOTTOM || y + r >= WINDOW_TOP)
	{
		return true;
	}

	return false;
}
void checkAndHandleLightsCollisions()
{
	for (int i = 0; i < 4; i++)
	{
		if (checkLightVertCol(plLights[i]))
		{
			plLights[i].velocity.x *= -1;
			plLights[i].position.x = clamp(plLights[i].position.x, WINDOW_LEFT + plLights[i].radius, WINDOW_RIGHT - plLights[i].radius);
		}

		if (checkLightHorzCol(plLights[i]))
		{
			plLights[i].velocity.y *= -1;
			plLights[i].position.y = clamp(plLights[i].position.y, GAME_AREA_BOTTOM + plLights[i].radius, WINDOW_TOP - plLights[i].radius);
		}
	}
}
void updateGame(double deltaTime)
{
	// deltatime is in milliseconds.

	updatePlanets(deltaTime);
	checkAndHandleLightsCollisions();
	updateLights(deltaTime);
}
vec_t randVect(vec_t min, vec_t max)
{
	return { randomDouble(min.x, max.x), randomDouble(min.y, max.y) };
}
vec_t randomLightVelocity()
{
	polar_t p;
	p.magnitude = randomDouble(MIN_LIGHT_SPEED, MAX_LIGHT_SPEED);
	p.angle = randomDouble(0, 360);
	return pol2rec(p);
}
void initLights()
{
	plLights[0].active = true;
	plLights[0].position = { 0, 0 };
	plLights[0].velocity = { 0, 0 };
	plLights[0].radius = SUN_RADIUS;

	for (int i = 1; i < 4; i++)
	{
		plLights[i].active = true;
		plLights[i].position = randVect(GAME_AREA_MIN, GAME_AREA_MAX);
		plLights[i].velocity = randomLightVelocity();
		plLights[i].radius = LIGHT_RADIUS;
	}

	plLights[0].color = { 1,1,1 };
	plLights[3].color = { 1, 0, 0 };
	plLights[1].color = { 0, 1, 0 };
	plLights[2].color = { 0, 0, 1 };
}
void initPlanet(planet_t* planet, int orbitRadius)
{
	// random angular velocity
	planet->angularVel = randomDouble(MIN_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);

	// random position
	planet->position = rotate(makeVector(orbitRadius, 0), randomDouble(0, 360));

	// random radius
	planet->radius = randomDouble(PLANET_MIN_RADIUS, PLANET_MAX_RADIUS);
}
void initPlanets()
{
	double nextRad = FIRST_ORBIT_RADIUS;

	for (int i = 0; i < ORBIT_COUNT; i++)
	{
		initPlanet(&planets[i], nextRad);
		nextRad += ORBIT_RADIUS_INCREMENT;
	}
}

void initNewSession()
{
	initPlanets();
	initLights();
	plLightRange = INITIAL_LIGHT_RANGE;
}

void stars()
{
	int i;
	srand(time(0));

	for (i = 0; i < 200; i++)
	{
		int xS = rand() % (800 - (-800) + 1) + (-800);
		int yS = rand() % (700 - (-700) + 1) + (-700);
		glColor3ub(255, 215, 0);
		glBegin(GL_LINES);
		// line 1
		glVertex2f(0 + xS, 5 + yS);
		glVertex2f(0 + xS, -5 + yS);
		//line2
		glVertex2f(-5 + xS, 0 + yS);
		glVertex2f(5 + xS, 0 + yS);
		//line3
		glVertex2f(-3 + xS, 4 + yS);
		glVertex2f(4 + xS, -3 + yS);
		//line4
		glVertex2f(-4 + xS, -3 + yS);
		glVertex2f(3 + xS, 4 + yS);
		glEnd();
	}

}
void drawPlanet(const planet_t cel)
{
	glBegin(GL_TRIANGLE_FAN);

	polar_t relVertPolar;
	relVertPolar.angle = 0;
	relVertPolar.magnitude = cel.radius;

	while (relVertPolar.angle <= 360)
	{
		vec_t relVertPos = pol2rec(relVertPolar);
		vec_t vertPos = addV(relVertPos, cel.position);

		vertex_t vert;
		vert.pos = vertPos;
		vert.normal_pos = unitV(subV(vertPos, cel.position));

		setColor(calculateTotalColor(vert));
		setVertex(vert.pos);
		relVertPolar.angle++;
	}

	glEnd();
}
void drawPlanets()
{
	for (int i = 0; i < ORBIT_COUNT; i++)
	{
		drawPlanet(planets[i]);
	}
}
void drawLights()
{
	for (int i = 0; i < 4; i++)
	{
		if (plLights[i].active)
		{
			setColor(plLights[i].color);
		}
		drawCircle(plLights[i].position.x, plLights[i].position.y, plLights[i].radius);
	}
}
void drawGameObjects()
{
	drawPlanets();
	drawLights();
}
void draw()
{
	drawGameObjects();
	stars();
}void bitmapStringWithVariables(const int lean, const int x, const int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len = (int)strlen(str);

	double leanShift = (lean + 1) * glutBitmapLength(font, (unsigned char*)str) / 2.0;
	glRasterPos2f(x - leanShift, y);

	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void drawStatusTexts()
{
	color_t red = { 1, 0.1, 0.1 };
	color_t green = { 0.1, 1, 0.1 };
	color_t blue = { 0.1, 1, 1 };

	setColor(red);
	bitmapStringWithVariables(-1, -390, -340, GLUT_BITMAP_8_BY_13, "<F1(Green): %s>", plLights[1].active ? "on" : "off");

	setColor(red);
	bitmapStringWithVariables(-1, -225, -340, GLUT_BITMAP_8_BY_13, "<F2(Blue): %s>", plLights[2].active ? "on" : "off");

	setColor(red);
	bitmapStringWithVariables(-1, -60, -340, GLUT_BITMAP_8_BY_13, "<F3(Red): %s>", plLights[3].active ? "on" : "off");
}
void display()
{
	color_t red = { 1, 0.1, 0.1 };
	color_t green = { 0.1, 1, 0.1 };
	color_t blue = { 0.1, 1, 1 };

	// Black
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Preparing the scheme
	draw();
	glColor3ub(0, 255, 255);
	vprint(-400, 325, GLUT_BITMAP_HELVETICA_12, "- ARINC DOGAN SENER -");

	glColor3ub(0, 255, 255);
	vprint(-400, 300, GLUT_BITMAP_HELVETICA_12, "\           22001998");

	//glColor3ub(136, 8, 8);
	//vprint(-390, -340, GLUT_BITMAP_HELVETICA_10, "<F1 For Change the Color of Green>");
	//glColor3ub(136, 8, 8);
	//vprint(-200, -340, GLUT_BITMAP_HELVETICA_10, "<F2 For Change the Color of Blue>");
	//glColor3ub(136, 8, 8);
	//vprint(0, -340, GLUT_BITMAP_HELVETICA_10, "<F3 For Change the Color of Red>");
	/*glColor3ub(136, 8, 8);*/
	setColor(red);
	vprint(120, -340, GLUT_BITMAP_8_BY_13, "<F4 Pause>");
	/*glColor3ub(136, 8, 8);*/
	setColor(red);
	vprint(275, -340, GLUT_BITMAP_8_BY_13, "<F5 Restart>");

	glColor3ub(255, 255, 255);
	vprint(200, 325, GLUT_BITMAP_HELVETICA_18, "UP Key for brighten up");
	glColor3ub(255, 255, 255);
	vprint(200, 300, GLUT_BITMAP_HELVETICA_18, "DOWN Key for dimming");

	drawCircleWire(0, 0, 100);

	drawCircleWire(0, 0, 200);

	drawCircleWire(0, 0, 300);

	drawStatusTexts();

	glutSwapBuffers();
}
void onResize(int w, int h)
{
	plWindowWidth = w;
	plWindowHeight = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1); // Set origin to the center of the screen
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	display();

}
void onKeyDown(const unsigned char key, const int x, const int y)
{
	switch (key)
	{
	case 27:
		exit(0); // Exit when ESC is pressed
		break;

	}

	// Refresh the view
	glutPostRedisplay();
}
// Timer
void onTimer_game(const int v)
{
	// Reschedule the next timer tick
	glutTimerFunc(TIMER_PERIOD, onTimer_game, 0);

	int now = NOW;
	int deltaTime = now - plTime;
	plTime = now;

	if (plRun)
	{
		updateGame(deltaTime);
	}

	// Refresh the view
	glutPostRedisplay();
}
void handleMouseMove(int x, int y)
{
	//   x2 = x1 - winWidth / 2
	//   y2 = winHeight / 2 - y1
	int x2 = x - plWindowWidth / 2;
	int y2 = plWindowHeight / 2 - y;

}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key)
	{
	case GLUT_KEY_F1:
		plLights[1].active = !plLights[1].active;
		break;

	case GLUT_KEY_F2:
		plLights[2].active = !plLights[2].active;
		break;

	case GLUT_KEY_F3:
		plLights[3].active = !plLights[3].active;
		break;

	case GLUT_KEY_F4:
		plRun = !plRun;
		break;

	case GLUT_KEY_F5:
		initNewSession();

	case GLUT_KEY_UP:
		plLightRange += 250;
		break;

	case GLUT_KEY_DOWN:
		plLightRange = clampAbove0(plLightRange - 250);
		break;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}
void main(int argc, char* argv[])
{
	srand(time(0));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(TOTAL_WIDTH, TOTAL_HEIGHT);
	glutInitWindowPosition(400, 20);
	glutCreateWindow("CTIS 164 Homework 3 - Arınç Doğan Şener - Colorful Planets");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);
	glutSpecialUpFunc(onSpecialKeyDown);

	initNewSession();
	plRun = true;

	// Making the shapes handsome
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutTimerFunc(TIMER_PERIOD, onTimer_game, 0);

	glutMainLoop();
}