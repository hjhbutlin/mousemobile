// ========================= CONFIG =========================

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

#define WINDOW_NAME "eggs"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

const double BACKGROUND_COLOUR[4] = {1.0f, 1.0f, 1.0f, 1.0f};

#define RENDER_MPF 5 // milliseconds per frame; 20 => 50fps

#define WHEEL_WIDTH 10
#define WHEEL_HEIGHT 16
#define CAR_WIDTH 40
#define CAR_HEIGHT 100

#define CAR_BASE_SPEED 5.0

#define CAR_ACCEL 0

// Camera settings
#define CAMERA_SMOOTHNESS 1.0f  // Lower = smoother (range 0.01-1.0)

typedef struct {
    double x, y, speed, wheelAngle, angle; // centre (x,y), speed scalar, wheel angle, angle
} Car;

Car you = {0.0, 0.0, CAR_BASE_SPEED, 0.0, 0.0};

// Camera position
double cameraX = 0.0;
double cameraY = 0.0;
double cameraSpeed = 0.0; // Track camera speed to handle deceleration

// window input
int mouseDown = 0;

// ==========================================================







// ======================= UTILITIES ========================

// Rotate a point around an origin
void rotatePoint(double x, double y, double cx, double cy, double angle, double *rx, double *ry) {
    double s = sin(angle);
    double c = cos(angle);

    // Translate point to origin
    double x0 = x - cx;
    double y0 = y - cy;

    // Rotate point
    *rx = cx + x0 * c - y0 * s;
    *ry = cy + x0 * s + y0 * c;
}

// pythag for distance
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void drawCar() {
    glColor3f(0.0,0.0,0.0);
    
    // define car vertices in car frame of ref
    float wheelVertices[4][2] = {
        {-WHEEL_WIDTH, -WHEEL_HEIGHT},
        {WHEEL_WIDTH, -WHEEL_HEIGHT},
        {WHEEL_WIDTH, WHEEL_HEIGHT},
        {-WHEEL_WIDTH, WHEEL_HEIGHT},
    };

    // save the current matrix
    glPushMatrix();
    
    // move to car position
    glTranslatef(you.x, you.y, 0.0f);
    
    // rotate entire car by car angle (conversion to degrees for glRotatef)
    glRotatef(-you.angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);

    // draw the car body
    glBegin(GL_QUADS);
    
    // GRAY BIT
    glColor3f(0.4,0.4,0.4);

    // lower bit
    glVertex2f(CAR_WIDTH*0.8, -0.867 * CAR_HEIGHT);
    glVertex2f(CAR_WIDTH*0.9, 0);
    glVertex2f(-CAR_WIDTH*0.9, 0);
    glVertex2f(-CAR_WIDTH*0.8, -0.867 * CAR_HEIGHT);

    // wheel axel bit
    glVertex2f(CAR_WIDTH*0.2, 0.4 * CAR_HEIGHT);
    glVertex2f(CAR_WIDTH, 0.64 * CAR_HEIGHT);
    glVertex2f(-CAR_WIDTH, 0.64 * CAR_HEIGHT);
    glVertex2f(-CAR_WIDTH*0.2, 0.4 * CAR_HEIGHT);

    // MAIN RED BIT
    glColor3f(1.0,0.0,0.0);

    // spoiler
    glVertex2f(-CAR_WIDTH, -CAR_HEIGHT);
    glVertex2f(CAR_WIDTH, -CAR_HEIGHT);
    glVertex2f(CAR_WIDTH, -0.867 * CAR_HEIGHT);
    glVertex2f(-CAR_WIDTH, -0.867 * CAR_HEIGHT);

    // rear section
    glVertex2f(CAR_WIDTH*0.5, -0.867 * CAR_HEIGHT);
    glVertex2f(CAR_WIDTH*0.9, 0);
    glVertex2f(-CAR_WIDTH*0.9, 0);
    glVertex2f(-CAR_WIDTH*0.5, -0.867 * CAR_HEIGHT);

    // wheel long bit
    glVertex2f(CAR_WIDTH*0.37, 0);
    glVertex2f(CAR_WIDTH*0.25, CAR_HEIGHT*0.9);
    glVertex2f(-CAR_WIDTH*0.25, CAR_HEIGHT*0.9);
    glVertex2f(-CAR_WIDTH*0.37, 0);

    // left nose cone section
    glVertex2f(-CAR_WIDTH, CAR_HEIGHT*0.85);
    glVertex2f(-CAR_WIDTH, CAR_HEIGHT*0.95);
    glVertex2f(0, CAR_HEIGHT);
    glVertex2f(-0.125 * CAR_WIDTH, CAR_HEIGHT*0.85);

    // right nose cone section
    glVertex2f(-0.125 * CAR_WIDTH, CAR_HEIGHT*0.85);
    glVertex2f(0, CAR_HEIGHT);
    glVertex2f(CAR_WIDTH, CAR_HEIGHT*0.95);
    glVertex2f(CAR_WIDTH, CAR_HEIGHT*0.85);
    
    glEnd();

    // driver's helmet
    glColor3f(0.2,0.4,1.0);
    glBegin(GL_TRIANGLE_FAN);
    double cx = 0;
    double cy = -0.2*CAR_HEIGHT;
    glVertex2f(cx, cy);
    for (int i = 0; i <= 10; i++) { 
        double angle = 2.0f * M_PI * i / 10;
        double x = cx + 0.333*CAR_WIDTH * cos(angle);
        double y = cy + 0.333*CAR_WIDTH * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // WHEELS
    glColor3f(0.0,0.0,0.0);
    
    // front wheels rotate
    glPushMatrix();
    glTranslatef(-CAR_WIDTH, 0.6 * CAR_HEIGHT, 0.0f);
    glRotatef(-you.wheelAngle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    for (int f = 0; f < 4; f++) {
        glVertex2f(wheelVertices[f][0], wheelVertices[f][1]);
    }
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(CAR_WIDTH, 0.6 * CAR_HEIGHT, 0.0f);
    glRotatef(-you.wheelAngle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    for (int f = 0; f < 4; f++) {
        glVertex2f(wheelVertices[f][0], wheelVertices[f][1]);
    }
    glEnd();
    glPopMatrix();
    
    // back wheels don't rotate
    glPushMatrix();
    glTranslatef(-CAR_WIDTH, -0.75 * CAR_HEIGHT, 0.0f);
    glBegin(GL_QUADS);
    for (int f = 0; f < 4; f++) {
        glVertex2f(wheelVertices[f][0], wheelVertices[f][1]);
    }
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(CAR_WIDTH, -0.75 * CAR_HEIGHT, 0.0f);
    glBegin(GL_QUADS);
    for (int f = 0; f < 4; f++) {
        glVertex2f(wheelVertices[f][0], wheelVertices[f][1]);
    }
    glEnd();
    glPopMatrix();
    
    // restore the matrix
    glPopMatrix();
}

// grid for testing
void drawGrid() {
    int gridSize = 2000;  // Make the grid large enough
    int gridStep = 50;    // Space between grid lines
    
    glColor3f(0.8, 0.8, 0.8);  // Light gray
    glLineWidth(1.0);
    
    glBegin(GL_LINES);
    
    // Vertical lines
    for (int x = -gridSize; x <= gridSize; x += gridStep) {
        glVertex2f(x, -gridSize);
        glVertex2f(x, gridSize);
    }
    
    // Horizontal lines
    for (int y = -gridSize; y <= gridSize; y += gridStep) {
        glVertex2f(-gridSize, y);
        glVertex2f(gridSize, y);
    }
    
    glEnd();
}

// ===========================================================






// ===================== RENDER PIPELINE =====================

void initOpenGL() {
    double r = BACKGROUND_COLOUR[0]; 
    double g = BACKGROUND_COLOUR[1]; 
    double b = BACKGROUND_COLOUR[2]; 
    double a = BACKGROUND_COLOUR[3];
    
    glClearColor(r, g, b, a);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up the camera view
    glLoadIdentity();
    glScalef(0.5,0.5,0.5);

    // glTranslatef(-you.x, -you.y, 0.0f);
    
    // Draw the grid to give a sense of movement
    drawGrid();
    
    // Draw the car
    drawCar();

    glFlush();
    glutSwapBuffers();
}

void update(int n) {
    (void)n;

    you.speed += CAR_ACCEL; // steady acceleration
    
    if (fabs(you.wheelAngle) > 0.001) {
        double wheelbase = 1.35 * CAR_HEIGHT; // length of wheelbase
        
        // Ackermann steering geometry approximation
        double turnRadius = wheelbase / tan(fabs(you.wheelAngle));
        
        double angularVelocity = you.speed / turnRadius;
        
        if (you.wheelAngle < 0) {
            angularVelocity = -angularVelocity;
        }
        
        you.angle += angularVelocity;
    }

    you.x += you.speed * sin(you.angle);
    you.y += you.speed * cos(you.angle);
    
    glutPostRedisplay();
    glutTimerFunc(RENDER_MPF, update, 0);
}

// ===========================================================






// ====================== WINDOW INPUT =======================

void mouseMove(int x, int y) {
    (void)y;
    you.wheelAngle = ((float)x/WINDOW_WIDTH - 0.5);
}

void mousePress(int button, int mouseState, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (mouseState == GLUT_DOWN) {
            mouseDown = 1;
        } else if (mouseState == GLUT_UP) {   
            mouseDown = 0;
        }
    }
}

// ===========================================================






// ========================= MAIN ============================

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(WINDOW_NAME);
    
    glutMouseFunc(mousePress);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
    
    initOpenGL();
    glutTimerFunc(RENDER_MPF, update, 0);
    glutDisplayFunc(display);
    
    glutMainLoop();
    return 0;
}