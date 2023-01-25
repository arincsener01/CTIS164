/*********
   CTIS164 - Template Source Program
----------
STUDENT : Arýnç Doðan Þener
SECTION : 001
HOMEWORK: HW1
----------
PROBLEMS: If I don't generate 10 objects with left click, let's say I generated 5 objects, at the (0,0) a new object is getting generated on its own after firing.
----------
ADDITIONAL FEATURES:
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

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
// Modes
#define STOP             0
#define ACTION           1


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

int x, y;

int count = 0;
int xB, yB;
int sCnt = 0, r = 0;
int mx, my;
bool activeTimer = false,
activeTimer2 = false;
int mode = STOP;

typedef struct
{
    int x1, y1, count;
}loc_t;

loc_t location[10];
//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
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

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
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

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
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

//
// To display onto window using OpenGL commands
//

void drawship() //creating an object (ship)
{
    glColor3f(1, 0.64, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-600, 40);
    glVertex2f(-650, 40);
    glVertex2f(-625, 10);
    glEnd();

    glColor3f(1, 0.64, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-600, 150);
    glVertex2f(-650, 150);
    glVertex2f(-625, 180);
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-650, 150);
    glVertex2f(-600, 150);
    glVertex2f(-600, 40);
    glVertex2f(-650, 40);
    glEnd();

    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(-640, 130);
    glVertex2f(-610, 130);
    glVertex2f(-610, 60);
    glVertex2f(-640, 60);
    glEnd();

    glColor3f(0, 0, 1);
    circle(-625, 75, 10);

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-635, 100);
    glVertex2f(-630, 110);
    glVertex2f(-625, 110);
    glVertex2f(-620, 100);
    glVertex2f(-625, 90);
    glVertex2f(-630, 90);
    glEnd();

}
void drawRectangle() //this basically our whole background
{
    int i = 0;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.4, 0.4, 0.4);
    glRectf(-500, 600, 500, -600);

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(500, 300);
    glVertex2f(500, 225);
    glVertex2f(700, 225);
    glVertex2f(700, 300);
    glEnd();

    glColor3f(1, 1, 1);
    vprint(525, 275, GLUT_BITMAP_8_BY_13, "ARINC DOGAN SENER");
    glColor3f(1, 1, 1);
    vprint(525, 250, GLUT_BITMAP_8_BY_13, "ID:22001998");

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-500, -300);
    glVertex2f(-500, -225);
    glVertex2f(-700, -225);
    glVertex2f(-700, -300);
    glEnd();

    glColor3f(1, 1, 1);
    vprint(-685, -250, GLUT_BITMAP_8_BY_13, "CLICK SCREEN TO CREATE ");
    glColor3f(1, 1, 1);
    vprint(-685, -265, GLUT_BITMAP_8_BY_13, "OBJECTS (MAX:10)");
    glColor3f(1, 1, 1);
    vprint(-685, -280, GLUT_BITMAP_8_BY_13, "-----------------");
    glColor3f(1, 1, 1);
    vprint(-685, -295, GLUT_BITMAP_8_BY_13, "<F1> for fire");

    glColor3f(1, 1, 1);
    glRectf(-10, 50, 10, -50);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(-10, 250);
    glVertex2f(-10, 150);
    glVertex2f(10, 150);
    glVertex2f(10, 250);
    glEnd();

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(-10, -250);
    glVertex2f(-10, -150);
    glVertex2f(10, -150);
    glVertex2f(10, -250);
    glEnd();

    glColor3f(1, 1, 1);
    glRectf(-290, 50, -270, -50);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(-290, 250);
    glVertex2f(-290, 150);
    glVertex2f(-270, 150);
    glVertex2f(-270, 250);
    glEnd();

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(-290, -250);
    glVertex2f(-290, -150);
    glVertex2f(-270, -150);
    glVertex2f(-270, -250);
    glEnd();

    glColor3f(1, 1, 1);
    glRectf(290, 50, 270, -50);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(290, 250);
    glVertex2f(290, 150);
    glVertex2f(270, 150);
    glVertex2f(270, 250);
    glEnd();

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(290, -250);
    glVertex2f(290, -150);
    glVertex2f(270, -150);
    glVertex2f(270, -250);
    glEnd();

    /*  for (i = 0; i < 3; i++)
      {
          int k = 0;

          glColor3f(1, 1, 1);
          glBegin(GL_QUADS);
          glVertex2f(-10, -250 + k);
          glVertex2f(-10, -150 + k);
          glVertex2f(10, -150 + k);
          glVertex2f(10, -250 + k);
          glEnd();

          k += 100;
      }*/

    glColor3d(0.8, 0.8, 0.8);
    glRectf(-475, 400, -450, -400);

    glColor3d(0.8, 0.8, 0.8);
    glRectf(475, 400, 450, -400);


}
void drawcar() //our main object. drawing a car with all the necessary conversitions
{
    for (int q = 1; q <= 10; q++) {
        xB = (location + q - 1)->x1;
        yB = (location + q - 1)->y1;

        if (!(xB >= location[q].x1 && xB <= location[q].x1 &&
            yB >= location[q].y1 && yB <= location[q].y1)) {

            glColor3d(1, 0, 0);//deðiþmesi lazým
            //glRectf(xB + 35, yB - 60, xB - 35, yB + 60);
            glBegin(GL_QUADS);
            glVertex2f(xB + 35, yB + 60);
            glVertex2f(xB - 35, yB + 60);
            glVertex2f(xB - 35, yB - 60);
            glVertex2f(xB + 35, yB - 60);
            glEnd();

            glColor3f(0, 0, 0);
            glRectf(xB + 35, yB - 40, xB - 35, yB + 40);

            glColor3f(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2f(xB + 35, yB + 30);
            glVertex2f(xB + 40, yB + 30);
            glVertex2f(xB + 40, yB + 45);
            glVertex2f(xB + 35, yB + 45);
            glEnd();

            glColor3f(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2f(xB - 35, yB + 30);
            glVertex2f(xB - 40, yB + 30);
            glVertex2f(xB - 40, yB + 45);
            glVertex2f(xB - 35, yB + 45);
            glEnd();

            glColor3f(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2f(xB + 35, yB - 30);
            glVertex2f(xB + 40, yB - 30);
            glVertex2f(xB + 40, yB - 45);
            glVertex2f(xB + 35, yB - 45);
            glEnd();

            glColor3f(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2f(xB - 35, yB - 30);
            glVertex2f(xB - 40, yB - 30);
            glVertex2f(xB - 40, yB - 45);
            glVertex2f(xB - 35, yB - 45);
            glEnd();

            glColor3f(0, 0, 1);
            glBegin(GL_QUADS);
            glVertex2f(xB + 35, yB + 10);
            glVertex2f(xB + 35, yB + 30);
            glVertex2f(xB - 35, yB + 30);
            glVertex2f(xB - 35, yB + 10);
            glEnd();

            glColor3f(0.7, 0.7, 0.7);
            glBegin(GL_TRIANGLES);
            glVertex2f(xB, yB + 50);
            glVertex2f(xB - 5, yB + 60);
            glVertex2f(xB + 5, yB + 60);
            glEnd();

            glColor3f(1, 1, 1);
            circle(xB - 30, yB + 55, 5);

            glColor3f(1, 1, 1);
            circle(xB + 30, yB + 55, 5);
        }
    }
    //printf("%d %d\n", x, y);
}
void display() { //display function. I draw everything in here
    //
    // clear window to black
    //
    glClearColor(0, 0.6, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);


    drawRectangle();

    drawship();

    //if(count>0)
    drawcar();

    /*for (int y = 0; y < 10; y++) {
        if (location[y].x1 > 590)
            location[y].x1 = -585;

        if (location[y].y1 > 445)
            location[y].y1 = -450;
    }*/

    glutSwapBuffers();
}
//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)//important part. With f1 key we are firing the objects.
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }
    if (key == GLUT_KEY_F1) {
        if (mode == STOP) {
            mode = ACTION;
            activeTimer = true;
            sCnt = 0;
        }
        else {
            mode = STOP;
            activeTimer = false;
        }
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)//with left click I am generating objects in here. sCnt variable is very important on this part
{
    xB = x - winWidth / 2;
    yB = winHeight / 2 - y;
    // Write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && sCnt < 10)
    {
        if (xB < 410 && xB > -410) {
            (*(location + sCnt)).count = sCnt;
            (*(location + sCnt)).x1 = xB;
            (*(location + sCnt)).y1 = yB;

            count++;
            sCnt++;
            r++;
        }


        /*location.x1 = x - winWidth / 2;
        location.y1 = winHeight / 2 - y;*/


    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.

    mx = x - winWidth / 2;
    my = winHeight / 2 - y;


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) { //active timer function here

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    // Write your codes here.
    if (activeTimer)
        for (int q = 0; q < 10; q++)
            location[q].y1 += 9;

    if (activeTimer2)//not worth of looking I planned something else but I ended up wont using it.
        for (int t = 0; t < 10; t++) {
            location[t].x1 += 9;
            location[t].y1 += 9;
        }


    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Highway to Hell");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}